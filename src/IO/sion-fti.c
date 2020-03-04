#include "../interface.h"


int FTI_SionClose(void *fileDesc){
    WriteSionInfo_t *fd = (WriteSionInfo_t *) fileDesc;
    if (sion_parclose_mapped_mpi(fd->sid) == -1) {
        FTI_Print("Cannot close sionlib file.", FTI_WARN);
        free(fd->file_map);
        free(fd->rank_map);
        free(fd->ranks);
        free(fd->chunkSizes);
        return FTI_NSCS;
    }
    free(fd->file_map);
    free(fd->rank_map);
    free(fd->ranks);
    free(fd->chunkSizes);
    return FTI_SCES;
}


/*-------------------------------------------------------------------------*/
/**
  @brief      Initializes the files for the upcoming checkpoint.  
  @param      FTI_Conf          Configuration of FTI 
  @param      FTI_Exec          Execution environment options 
  @param      FTI_Topo          Topology of nodes
  @param      FTI_Ckpt          Checkpoint configurations
  @param      FTI_Data          Data to be stored
  @return     void*             Return void pointer to file descriptor 

 **/
/*-------------------------------------------------------------------------*/
void *FTI_InitSion(FTIT_configuration* FTI_Conf, FTIT_execution* FTI_Exec, FTIT_topology* FTI_Topo, FTIT_checkpoint *FTI_Ckpt, FTIT_dataset *FTI_Data)
{
    WriteSionInfo_t *write_info = (WriteSionInfo_t *) malloc (sizeof(WriteSionInfo_t));

    write_info->loffset = 0;
    int numFiles = 1;
    int nlocaltasks = 1;
    write_info->file_map = calloc(1, sizeof(int));
    write_info->ranks = talloc(int, 1);
    write_info->rank_map = talloc(int, 1);
    write_info->chunkSizes = talloc(sion_int64, 1);
    int fsblksize = -1;
    write_info->chunkSizes[0] = FTI_Exec->ckptSize;
    write_info->ranks[0] = FTI_Topo->splitRank;
    write_info->rank_map[0] = FTI_Topo->splitRank;
    // open parallel file
    char fn[FTI_BUFS], str[FTI_BUFS];
    snprintf(str, FTI_BUFS, "Ckpt%d-sionlib.fti", FTI_Exec->ckptID);
    snprintf(fn, FTI_BUFS, "%s/%s", FTI_Conf->gTmpDir, str);

    snprintf(FTI_Exec->meta[0].ckptFile, FTI_BUFS, "%s",str);

    write_info->sid = sion_paropen_mapped_mpi(fn, "wb,posix", &numFiles, FTI_COMM_WORLD, &nlocaltasks, &write_info->ranks, &write_info->chunkSizes, &write_info->file_map, &write_info->rank_map, &fsblksize, NULL);

    // check if successful
    if (write_info->sid == -1) {
        errno = 0;
        FTI_Print("SIONlib: File could no be opened", FTI_EROR);

        free(write_info->file_map);
        free(write_info->rank_map);
        free(write_info->ranks);
        free(write_info->chunkSizes);
        free(write_info);
        return NULL;
    }

    // set file pointer to corresponding block in sionlib file
    int res = sion_seek(write_info->sid, FTI_Topo->splitRank, SION_CURRENT_BLK, SION_CURRENT_POS);

    // check if successful
    if (res != SION_SUCCESS) {
        errno = 0;
        FTI_Print("SIONlib: Could not set file pointer", FTI_EROR);
        sion_parclose_mapped_mpi(write_info->sid);
        free(write_info->file_map);
        free(write_info->rank_map);
        free(write_info->ranks);
        free(write_info->chunkSizes);
        free(write_info);
        return NULL;
    }
    return write_info;
}

/*-------------------------------------------------------------------------*/
/**
  @brief     Writes data to a file using the SION library
  @param     src    The location of the data to be written 
  @param     size   The number of bytes that I need to write 
  @param     opaque A pointer to the file descriptor  
  @return    integer FTI_SCES if successful.

  Writes the data to a file using the SION library. 

 **/
/*-------------------------------------------------------------------------*/
int FTI_SionWrite (void *src, size_t size, void *opaque)
{
    int *sid= (int *)opaque;
    int res = sion_fwrite(src, size, 1, *sid);
    if (res < 0 ){
        return FTI_NSCS;
    }
    return FTI_SCES;
}

/*-------------------------------------------------------------------------*/
/**
  @brief      Writes ckpt to using POSIX format.
  @param      FTI_Conf        Configuration metadata.
  @param      FTI_Exec        Execution metadata.
  @param      FTI_Topo        Topology metadata.
  @param      FTI_Ckpt        Checkpoint metadata.
  @param      FTI_Data        Dataset metadata.
  @return     integer         FTI_SCES if successful.

 **/
/*-------------------------------------------------------------------------*/

int FTI_WriteSionData(FTIT_dataset *FTI_DataVar, void *fd){
    WriteSionInfo_t *write_info = (WriteSionInfo_t*) fd;
    int res;
    char str[FTI_BUFS];
    FTI_Print("Writing Sion Data",FTI_INFO);
    if ( !FTI_DataVar->isDevicePtr) {
        res = FTI_SionWrite(FTI_DataVar->ptr, FTI_DataVar->size, &write_info->sid);
        if (res != FTI_SCES){
            snprintf(str, FTI_BUFS, "Dataset #%d could not be written.", FTI_DataVar->id);
            FTI_Print(str, FTI_EROR);
            errno = 0;
            FTI_Print("SIONlib: Data could not be written", FTI_EROR);
            res =  sion_parclose_mapped_mpi(write_info->sid);
            free(write_info->file_map);
            free(write_info->rank_map);
            free(write_info->ranks);
            free(write_info->chunkSizes);
            return FTI_NSCS;
        }
    }
#ifdef GPUSUPPORT            
    // if data are stored to the GPU move them from device
    // memory to cpu memory and store them.
    else {
        if ((res = FTI_Try(
                        TransferDeviceMemToFileAsync(&FTI_Data[i], FTI_SionWrite, &write_info->sid),
                        "moving data from GPU to storage")) != FTI_SCES) {
            snprintf(str, FTI_BUFS, "Dataset #%d could not be written.", FTI_DataVar->id);
            FTI_Print(str, FTI_EROR);
            errno = 0;
            FTI_Print("SIONlib: Data could not be written", FTI_EROR);
            res =  sion_parclose_mapped_mpi(write_info->sid);
            free(write_info->file_map);
            free(write_info->rank_map);
            free(write_info->ranks);
            free(write_info->chunkSizes);
            return FTI_NSCS;
        }
    }
#endif            
    write_info->loffset+= FTI_DataVar->size;
    return FTI_SCES;

}


size_t FTI_GetSionFilePos(void *fileDesc){
    WriteSionInfo_t *fd  = (WriteSionInfo_t *) fileDesc;
    return fd->loffset;
}

/*-------------------------------------------------------------------------*/
/**
  @brief      Initializes variable recovery for SION mode
  @param      fn                        ckpt file
  @return     Integer                   SION file handle
                                        positive if successful
                                        -1 if not successful
 **/
/*-------------------------------------------------------------------------*/
int FTI_RecoverVarInitSION(char* fn)//should be in src/IO/sion-fti.c
{ 
    int numFiles = 1;
    MPI_Comm gComm = FTI_COMM_WORLD;
    MPI_Comm lComm = FTI_COMM_WORLD;
    sion_int64 chunksize = 10 * 1024 * 1024; //maximum size to write 
    sion_int32 fsblksize = -1;               //file system block size. (-1 for automatic) 
    int globalrank = FTI_Topo.splitRank;     //splitRank(FTI) instead of myRank(MPI)
    FILE* fileptr = NULL;
    char *newfname = NULL;

    //open file for read
    int fileHandle = sion_paropen_mpi(fn, "rb", &numFiles, gComm, &lComm, &chunksize, &fsblksize, &globalrank, &fileptr, &newfname);

    if(fileHandle == -1){
        FTI_Print("Could not open FTI checkpoint file.", FTI_EROR);
    }
    return fileHandle;
}

/*-------------------------------------------------------------------------*/
/**
  @brief      Recovers variable for SION mode
  @param      id                        variable id
  @param      Integer                   fileHandle                
  @return     Integer                   FTI_SCES if successful 
                                        
 **/
/*-------------------------------------------------------------------------*/
int FTI_RecoverVarSION(int id, int fileHandle)//should be in src/IO/sion-fti.c
{
    //current position?
    int res = FTI_NSCS; 

    //SION_CURRENT_POS -> filePose(long) : convert to sion_int64
    int activeID, oldID;
    res = findVarInMeta(&FTI_Exec, FTI_Data, id, &activeID, &oldID);
    if(res != FTI_NSCS){
        sion_int64 filePos = FTI_Exec.meta[FTI_Exec.ckptLvel].filePos[oldID]; 
        res = sion_seek(fileHandle, FTI_Topo->splitRank, SION_CURRENT_BLK, filePos);//assumes var resides in current block
        if(res == SION_SUCCESS){
            res = sion_fread(FTI_Data[activeID].ptr, 1, btoread, fileHandle);
            if(res <= 0){//no elements were read
                FTI_Print("Could not read variable.", FTI_EROR);
            }else{
                res = FTI_SCES;
            }
        }
    }
    return res; 
}

/*-------------------------------------------------------------------------*/
/**
  @brief      Finalizes variable recovery for SION mode
  @param      Integer                   fileHandle                
  @return     Integer                   FTI_SCES if successful 
                                        
 **/
/*-------------------------------------------------------------------------*/
int FTI_RecoverVarFinalizeSION(int fileHandle)//should be in src/IO/sion-fti.c
{
    int res = FTI_SCES;
    if(sion_parclose_mpi(fileHandle) != SION_SUCCESS){//returns SION_NOT_SUCCESS
        res = FTI_NSCS;
        FTI_Print("Could not close FTI checkpoint file.", FTI_EROR);
    }
    return res; 
}


