#include<stdio.h>
#include<fti.h>

int main (){

	long* array = calloc(arraySize, sizeof(long));
	FTI_Protect(1, array, arraySize, FTI_LONG);
	if (FTI_Status() != 0) {
		long arraySizeInBytes = FTI_GetStoredSize(1);
		if (arraySizeInBytes == 0) {
			printf("No stored size in metadata!\n");
			return GETSTOREDSIZE_FAILED;
		}
	array = realloc(array, arraySizeInBytes);
	int res = FTI_Recover();
	if (res != 0) {
		printf("Recovery failed!\n");
		return RECOVERY_FAILED;
	}
	//update arraySize
	arraySize = arraySizeInBytes / sizeof(long);
	}
	for (i = 0; i < max; i++) {
		if (i % CKTP_STEP) {
		//update FTI array size information
			FTI_Protect(1, array, arraySize, FTI_LONG);
			int res = FTI_Checkpoint((i % CKTP_STEP) + 1, 1);
			if (res != FTI_DONE) {
				printf("Checkpoint failed!.\n");
				return CHECKPOINT_FAILED;
			}
		}
		...
		//add element to array
		arraySize += 1;
		array = realloc(array, arraySize * sizeof(long));
	}
	return 0;

}
