#ifndef __TOPO_H__
#define __TOPO_H__

int FTI_SaveTopo( char *nameList );
int FTI_ReorderNodes( int *nodeList, char *nameList );
int FTI_BuildNodeList( int *nodeList, char *nameList );
int FTI_CreateGroupTopology( int * nodeList, int * group, int * distProcList );
int FTI_CreateComms( int *userProcList, int *distProcList, int* nodeList, int* group );
int FTI_Topology(void);

#endif // __TOPO_H__
