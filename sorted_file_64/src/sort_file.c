#include "sort_file.h"
#include "bf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(BF_Block **blocks, int left_block, int left_rec, int right_block, int right_rec){
	Record rec1, rec2;
	char *data_left, *data_right;

	data_left= BF_Block_GetData(blocks[left_block]);
	data_right= BF_Block_GetData(blocks[right_block]);
	memcpy(&rec1, data_left+sizeof(int)+left_rec*sizeof(Record), sizeof(Record));
	memcpy(&rec2, data_right+sizeof(int)+right_rec*sizeof(Record), sizeof(Record));
	memcpy(data_right+sizeof(int)+right_rec*sizeof(Record), &rec1, sizeof(Record));
	memcpy(data_left+sizeof(int)+left_rec*sizeof(Record), &rec2, sizeof(Record));
}

void partition(BF_Block **blocks, int left_block, int left_rec, int right_block, int right_rec, int pivot, int *partition_block, int *partition_rec){
	int left_block_pointer, left_rec_pointer, right_block_pointer, right_rec_pointer, right;
	char *data;
	Record rec;

	if(left_rec==0){
		left_block_pointer=left_block-1;	
		left_rec_pointer=BF_BLOCK_SIZE/sizeof(Record)-1;
	}
	else{
		left_block_pointer=left_block;
		left_rec_pointer=left_rec-1;
	}
	right_block_pointer=right_block;
	right_rec_pointer=right_rec;

	while(1){
		if(left_rec_pointer==(BF_BLOCK_SIZE/sizeof(Record)-1)){
			left_block_pointer++;
			left_rec_pointer=0;
		}
		else{
			left_rec_pointer++;
		}
		data= BF_Block_GetData(blocks[left_block_pointer]);
		memcpy(&rec, data+sizeof(int)+left_rec_pointer*sizeof(Record), sizeof(Record));
		while(rec.id<pivot){
			if(left_rec_pointer==(BF_BLOCK_SIZE/sizeof(Record)-1)){
				left_block_pointer++;
				left_rec_pointer=0;
				data= BF_Block_GetData(blocks[left_block_pointer]);
			}
			else{
				left_rec_pointer++;
			}
			memcpy(&rec, data+sizeof(int)+left_rec_pointer*sizeof(Record), sizeof(Record));
		}
		
		right=right_block_pointer*BF_BLOCK_SIZE/sizeof(Record)+right_rec_pointer;
		if(right>0){
			if(right_rec_pointer==0){
				right_block_pointer--;
				right_rec_pointer=BF_BLOCK_SIZE/sizeof(Record)-1;
			}
			else{
				right_rec_pointer--;
			}
			data= BF_Block_GetData(blocks[right_block_pointer]);
			memcpy(&rec, data+sizeof(int)+right_rec_pointer*sizeof(Record), sizeof(Record));
		}
		while(right>0 && rec.id>pivot){
			right--;
			if(right<=0){
				break;
			}
			if(right_rec_pointer==0){
				right_block_pointer--;
				right_rec_pointer=BF_BLOCK_SIZE/sizeof(Record)-1;
				data= BF_Block_GetData(blocks[right_block_pointer]);
			}
			else{
				right_rec_pointer--;
			}
			memcpy(&rec, data+sizeof(int)+right_rec_pointer*sizeof(Record), sizeof(Record));
		}

		if(left_block_pointer*BF_BLOCK_SIZE/sizeof(Record)+left_rec_pointer >= right){
			break;
		}
		else{
			swap(blocks, left_block_pointer, left_rec_pointer, right_block_pointer, right_rec_pointer);
		}
	}

	swap(blocks, left_block_pointer, left_rec_pointer, right_block, right_rec);

	*partition_block=left_block_pointer;
	*partition_rec=left_rec_pointer;
	
}

void quickSort(BF_Block **blocks, int left_block, int left_rec, int right_block, int right_rec){
	int right, left, pivot, partition_block, partition_rec;
	Record rec;
	char *data;	

	right= right_block*BF_BLOCK_SIZE/(int) sizeof(Record)+ right_rec;
	left= left_block*BF_BLOCK_SIZE/sizeof(Record)+ left_rec;
	if(right-left<=0){
		return;
	}
	else{
		data=BF_Block_GetData(blocks[right_block]);
		memcpy(&rec, data + sizeof(int)+ right_rec*sizeof(Record), sizeof(Record));
		pivot=rec.id;
		partition(blocks, left_block, left_rec, right_block, right_rec, pivot, &partition_block, &partition_rec);
		if(partition_rec==0){																						// kaloume tin quicksort apo left mexri partition_point-1
			quickSort(blocks, left_block, left_rec, partition_block-1, BF_BLOCK_SIZE/sizeof(Record)-1);
		}
		else{
			quickSort(blocks, left_block, left_rec, partition_block, partition_rec-1);
		}
		if(partition_rec==BF_BLOCK_SIZE/sizeof(Record)-1){															// kaloume tin quicksort apo partition_point+1 mexri right		
			quickSort(blocks, partition_block+1, 0, right_block, right_rec);
		}
		else{
			quickSort(blocks, partition_block, partition_rec+1, right_block, right_rec);
		}
	}	
}

void partition_char(BF_Block **blocks, int left_block, int left_rec, int right_block, int right_rec, char *pivot, int *partition_block, int *partition_rec, int fieldNo){
	int left_block_pointer, left_rec_pointer, right_block_pointer, right_rec_pointer, right, ret;
	char *data;
	Record rec;

	if(left_rec==0){
		left_block_pointer=left_block-1;	
		left_rec_pointer=BF_BLOCK_SIZE/sizeof(Record)-1;
	}
	else{
		left_block_pointer=left_block;
		left_rec_pointer=left_rec-1;
	}
	right_block_pointer=right_block;
	right_rec_pointer=right_rec;

	while(1){
		if(left_rec_pointer==(BF_BLOCK_SIZE/sizeof(Record)-1)){
			left_block_pointer++;
			left_rec_pointer=0;
		}
		else{
			left_rec_pointer++;
		}
		data= BF_Block_GetData(blocks[left_block_pointer]);
		memcpy(&rec, data+sizeof(int)+left_rec_pointer*sizeof(Record), sizeof(Record));
		if(fieldNo==1){
			ret=strcmp(rec.name, pivot);
		}
		else{
			ret=strcmp(rec.surname, pivot);
		}
		while(ret<0){
			if(left_rec_pointer==(BF_BLOCK_SIZE/sizeof(Record)-1)){
				left_block_pointer++;
				left_rec_pointer=0;
				data= BF_Block_GetData(blocks[left_block_pointer]);
			}
			else{
				left_rec_pointer++;
			}
			memcpy(&rec, data+sizeof(int)+left_rec_pointer*sizeof(Record), sizeof(Record));
			if(fieldNo==1){
				ret=strcmp(rec.name, pivot);
			}
			else{
				ret=strcmp(rec.surname, pivot);
			}
		}
		
		right=right_block_pointer*BF_BLOCK_SIZE/sizeof(Record)+right_rec_pointer;
		if(right>0){
			if(right_rec_pointer==0){
				right_block_pointer--;
				right_rec_pointer=BF_BLOCK_SIZE/sizeof(Record)-1;
			}
			else{
				right_rec_pointer--;
			}
			data= BF_Block_GetData(blocks[right_block_pointer]);
			memcpy(&rec, data+sizeof(int)+right_rec_pointer*sizeof(Record), sizeof(Record));
			if(fieldNo==1){
				ret=strcmp(rec.name, pivot);
			}
			else{
				ret=strcmp(rec.surname, pivot);
			}
		}
		while(right>0 && ret>0){
			right--;
			if(right<=0){
				break;
			}
			if(right_rec_pointer==0){
				right_block_pointer--;
				right_rec_pointer=BF_BLOCK_SIZE/sizeof(Record)-1;
				data= BF_Block_GetData(blocks[right_block_pointer]);
			}
			else{
				right_rec_pointer--;
			}
			memcpy(&rec, data+sizeof(int)+right_rec_pointer*sizeof(Record), sizeof(Record));
			if(fieldNo==1){
				ret=strcmp(rec.name, pivot);
			}
			else{
				ret=strcmp(rec.surname, pivot);
			}
		}

		if(left_block_pointer*BF_BLOCK_SIZE/sizeof(Record)+left_rec_pointer >= right){
			break;
		}
		else{
			swap(blocks, left_block_pointer, left_rec_pointer, right_block_pointer, right_rec_pointer);
		}
	}

	swap(blocks, left_block_pointer, left_rec_pointer, right_block, right_rec);

	*partition_block=left_block_pointer;
	*partition_rec=left_rec_pointer;
	
}

void quickSort_char(BF_Block **blocks, int left_block, int left_rec, int right_block, int right_rec, int fieldNo){
	int right, left, partition_block, partition_rec;
	Record rec;
	char *data, *pivot;	

	right= right_block*BF_BLOCK_SIZE/(int) sizeof(Record)+ right_rec;
	left= left_block*BF_BLOCK_SIZE/sizeof(Record)+ left_rec;
	if(right-left<=0){
		return;
	}
	else{
		data=BF_Block_GetData(blocks[right_block]);
		memcpy(&rec, data + sizeof(int)+ right_rec*sizeof(Record), sizeof(Record));
		if(fieldNo==1){
			pivot = malloc(sizeof(char) * strlen(rec.name));
			strcpy(pivot, rec.name);
		}
		else{
			pivot = malloc(sizeof(char) * strlen(rec.surname));
			strcpy(pivot, rec.surname);
		}
		partition_char(blocks, left_block, left_rec, right_block, right_rec, pivot, &partition_block, &partition_rec, fieldNo);
		if(partition_rec==0){																						// kaloume tin quicksort apo left mexri partition_point-1
			quickSort_char(blocks, left_block, left_rec, partition_block-1, BF_BLOCK_SIZE/sizeof(Record)-1, fieldNo);
		}
		else{
			quickSort_char(blocks, left_block, left_rec, partition_block, partition_rec-1, fieldNo);
		}
		if(partition_rec==BF_BLOCK_SIZE/sizeof(Record)-1){															// kaloume tin quicksort apo partition_point+1 mexri right
			quickSort_char(blocks, partition_block+1, 0, right_block, right_rec, fieldNo);
		}
		else{
			quickSort_char(blocks, partition_block, partition_rec+1, right_block, right_rec, fieldNo);
		}
		free(pivot);
	}
}


int power(int base, int exponent){
	int i, p;
	p=1;
	for(i=1; i<=exponent; i++){
		p=p*base;
	}
	return p;
}

SR_ErrorCode SR_Init() {

	//BF_Init(LRU);

  	return SR_OK;
}

SR_ErrorCode SR_CreateFile(const char *fileName) {
	BF_Block *block;
	int fileDesc, temp;
	char *data;

	if(BF_CreateFile(fileName) != BF_OK){
		return SR_ERROR;
	}
	BF_Block_Init(&block);
	if(BF_OpenFile(fileName, &fileDesc) != BF_OK){
		return SR_ERROR;
	}
	if(BF_AllocateBlock(fileDesc, block) != BF_OK){
		return SR_ERROR;
	}
	data=BF_Block_GetData(block);
	temp=0;
	memcpy(data, &temp, sizeof(int));
	temp=1;	
	memcpy(data+sizeof(int), &temp, sizeof(int));
	
	BF_Block_SetDirty(block);
	if(BF_UnpinBlock(block) != BF_OK){
		return SR_ERROR;
	}

  	return SR_OK;
}

SR_ErrorCode SR_OpenFile(const char *fileName, int *fileDesc) {
	BF_Block *block;
	int temp;
	char *data;
	
	if(BF_OpenFile(fileName, fileDesc) != BF_OK){
		return SR_ERROR;
	}
	BF_Block_Init(&block);
	if(BF_GetBlock(*fileDesc, 0, block) != BF_OK){
		return SR_ERROR;
	}
	data=BF_Block_GetData(block);
	
	memcpy(&temp, data + sizeof(int), sizeof(int));
	if(temp!=1){
		return SR_ERROR;
	}
	if(BF_UnpinBlock(block) != BF_OK){
		return SR_ERROR;
	}
  	return SR_OK;
}

SR_ErrorCode SR_CloseFile(int fileDesc) {
	if(BF_CloseFile(fileDesc) != BF_OK){
		return SR_ERROR;
	}
	
  	return SR_OK;
}

SR_ErrorCode SR_InsertEntry(int fileDesc, Record record) {
	int block_num, counter;
	char *data;
	BF_Block *block;
	BF_Block_Init(&block);
	
	if(BF_GetBlockCounter(fileDesc, &block_num) != BF_OK){
		return SR_ERROR;
	}
	if(block_num==1){
		if(BF_AllocateBlock(fileDesc, block) != BF_OK){
			return SR_ERROR;
		}
		data = BF_Block_GetData(block);
		counter=1;
		memcpy(data, &counter, sizeof(int));
		memcpy(data+sizeof(int), &record, sizeof(Record));
		BF_Block_SetDirty(block);
		if(BF_UnpinBlock(block) != BF_OK){
			return SR_ERROR;
		}
	}else{	
		if(BF_GetBlock(fileDesc, block_num-1, block) != BF_OK){
			return SR_ERROR;
		}
		data = BF_Block_GetData(block);
		memcpy(&counter, data, sizeof(int));
		if((counter+1)*sizeof(Record)+sizeof(int)>BF_BLOCK_SIZE){
			if(BF_UnpinBlock(block) != BF_OK){
				return SR_ERROR;
			}
			if(BF_AllocateBlock(fileDesc, block) != BF_OK){
				return SR_ERROR;
			}
			data = BF_Block_GetData(block);
			counter=1;			
			memcpy(data, &counter, sizeof(int));
			memcpy(data+sizeof(int), &record, sizeof(Record));
			BF_Block_SetDirty(block);
			if(BF_UnpinBlock(block) != BF_OK){
				return SR_ERROR;
			}
		}else{
			memcpy(data+sizeof(int)+counter*sizeof(Record), &record, sizeof(Record));
			counter++;
			memcpy(data, &counter, sizeof(int));
			BF_Block_SetDirty(block);
			if(BF_UnpinBlock(block) != BF_OK){
				return SR_ERROR;
			}
		}
	}
  	return SR_OK;
}

SR_ErrorCode SR_SortedFile(const char* input_filename, const char* output_filename, int fieldNo, int bufferSize){
	int i, j, z, w, fileDesc, fileDesc_temp1, fileDesc_output, fileDesc_temp2, blocks_num, bl_count, counter;
	int last, *bl_id, *recs, min, pos, ret, flag, all_blocks, m, sunolika_block;
	char *data, *data_temp1, *data_output;
	BF_Block **blocks, *block_input, *block_output;
	Record min_rec, cur_rec;
	
	if(bufferSize>BF_BUFFER_SIZE || bufferSize<3){	
		return SR_ERROR;	
	}
	if(SR_OpenFile(input_filename, &fileDesc) != SR_OK){					// Anoigoume to arxeio pros taxinomisi
		return SR_ERROR;
	}
	if(fieldNo==0){
		if(SR_CreateFile("temp_id1.db") != SR_OK){
			return SR_ERROR;
		}
		if(SR_OpenFile("temp_id1.db", &fileDesc_temp1) != SR_OK){
			return SR_ERROR;
		}
		if(SR_CreateFile("temp_id2.db") != SR_OK){
			return SR_ERROR;
		}
		if(SR_OpenFile("temp_id2.db", &fileDesc_temp2) != SR_OK){
			return SR_ERROR;
		}
	}
	else if(fieldNo==1){
		if(SR_CreateFile("temp_name1.db") != SR_OK){
			return SR_ERROR;
		}
		if(SR_OpenFile("temp_name1.db", &fileDesc_temp1) != SR_OK){	
			return SR_ERROR;
		}
		if(SR_CreateFile("temp_name2.db") != SR_OK){
			return SR_ERROR;
		}
		if(SR_OpenFile("temp_name2.db", &fileDesc_temp2) != SR_OK){	
			return SR_ERROR;
		}
	}
	else if(fieldNo==2){
		if(SR_CreateFile("temp_surname1.db") != SR_OK){
			return SR_ERROR;
		}
		if(SR_OpenFile("temp_surname1.db", &fileDesc_temp1) != SR_OK){
			return SR_ERROR;
		}
		if(SR_CreateFile("temp_surname2.db") != SR_OK){
			return SR_ERROR;
		}
		if(SR_OpenFile("temp_surname2.db", &fileDesc_temp2) != SR_OK){
			return SR_ERROR;
		}
	}

	if(BF_GetBlockCounter(fileDesc, &blocks_num) != BF_OK){					// Briskoyme posa blocks exei to arxiko
		return SR_ERROR;
	}
	BF_Block_Init(&block_input);
	BF_Block_Init(&block_output);
	for(i=1; i<blocks_num; i++){
		if(BF_GetBlock(fileDesc, i, block_input) != BF_OK){
			return SR_ERROR;
		}	
		if(BF_AllocateBlock(fileDesc_temp1, block_output) != BF_OK){
			return SR_ERROR;
		}
		data = BF_Block_GetData(block_input);
		data_temp1 = BF_Block_GetData(block_output);
		memcpy(&counter, data, sizeof(int));
		memcpy(data_temp1, &counter, sizeof(int));
		for(j=0; j<counter; j++){
			memcpy(data_temp1+sizeof(int)+j*sizeof(Record),data+sizeof(int)+j*sizeof(Record), sizeof(Record));
		}
		
		BF_Block_SetDirty(block_output);
		if(BF_UnpinBlock(block_input) != BF_OK){
			return SR_ERROR;
		}
		if(BF_UnpinBlock(block_output) != BF_OK){
			return SR_ERROR;
		}
	}

	blocks= malloc(bufferSize*sizeof(BF_Block *));
	for(i=0; i<bufferSize; i++){											// Kanoume init ta block pou desmeysame
		BF_Block_Init(&blocks[i]);
	}
	if(BF_GetBlockCounter(fileDesc_temp1, &blocks_num) != BF_OK){
		return SR_ERROR;
	}
	counter=1;
	while(blocks_num-1>0){													// Gia ola ta blocks dedomenwn
		if(blocks_num<bufferSize){
			bl_count=blocks_num-1;		
		}else{
			bl_count=bufferSize;
		}
		for(i=0; i<bl_count; i++){											// Ta pairnoume ana bufferSize *
			if(BF_GetBlock(fileDesc_temp1, counter+i, blocks[i]) != BF_OK){
				return SR_ERROR;
			}	
		}
		counter+=bl_count;
		data = BF_Block_GetData(blocks[bl_count-1]);
		memcpy(&last, data, sizeof(int));
		
		if(fieldNo==0){
			quickSort(blocks, 0, 0, bl_count-1, last-1);
		}
		else{
			quickSort_char(blocks, 0, 0, bl_count-1, last-1, fieldNo);
		}	
		for(i=0; i<bl_count; i++){
			BF_Block_SetDirty(blocks[i]);
			if(BF_UnpinBlock(blocks[i]) != BF_OK){
				return SR_ERROR;
			}
		}
		blocks_num-=bl_count;
	}
	flag=1;

	
		//		DEYTERO BHMA		//
/*	bl_id=malloc(bufferSize*sizeof(int));
	recs=malloc(bufferSize*sizeof(int));
	if(BF_GetBlockCounter(fileDesc_temp1, &blocks_num) != BF_OK){			// Briskoyme posa blocks exei
		return SR_ERROR;
	}
	blocks_num--;															// metrame mono block dedomenwn
	for(i=0; i<blocks_num; i++){
		if(BF_AllocateBlock(fileDesc_temp2, block_output) != BF_OK){
			return SR_ERROR;
		}
		if(BF_UnpinBlock(block_output) != BF_OK){
			return SR_ERROR;
		}
	}
	sunolika_block=blocks_num;
	flag=1; 
	if(blocks_num%bufferSize!=0){
		blocks_num=blocks_num/bufferSize;
		blocks_num++;
	}
	else{
		blocks_num=blocks_num/bufferSize;
	}
	m=0;
	while(blocks_num>1){
		bl_id[bufferSize-1]=1;
		recs[bufferSize-1]=0;
		if(BF_GetBlockCounter(fileDesc, &all_blocks) != BF_OK){
			return SR_ERROR;
		}
		all_blocks--;

		if(blocks_num%(bufferSize-1)==0){
			z=0;
		}
		else{
			z=1;
		}
		for(j=0; j<blocks_num/(bufferSize-1)+z; j++){
			min=bufferSize-1;													// arxikopoiisi deiktwn
			if(all_blocks%(bufferSize*power(bufferSize-1,m))==0){
				w=0;
			}
			else{
				w=1;
			}
			if(all_blocks/(bufferSize*power(bufferSize-1,m))+w < bufferSize-1){
				min=all_blocks/(bufferSize*power(bufferSize-1,m))+w;
			}
			for(i=0; i<min; i++){
				bl_id[i]=bufferSize*power(bufferSize-1,m)*((bufferSize-1)*j+i) + 1;
				recs[i]=0;
				all_blocks--;
				if(flag==1){
					if(BF_GetBlock(fileDesc_temp1, bl_id[i], blocks[i]) != BF_OK){
						return SR_ERROR;
					}	
				}
				else if(flag==2){
					if(BF_GetBlock(fileDesc_temp2, bl_id[i], blocks[i]) != BF_OK){
						return SR_ERROR;
					}	
				}	
			}
			bl_count=min;
			while(bl_count>0){
				if(flag==1){
					if(BF_GetBlock(fileDesc_temp2, bl_id[bufferSize-1], block_output) != BF_OK){
						return SR_ERROR;
					}
				}
				else if(flag==2){
					if(BF_GetBlock(fileDesc_temp1, bl_id[bufferSize-1], block_output) != BF_OK){
						return SR_ERROR;
					}
				}
				i=0;
				while(i<min){
					if(bl_id[i]<=(j*bufferSize*(bufferSize-1) + (i+1)*bufferSize)* power(bufferSize-1,m) && bl_id[i]<=sunolika_block){
						data = BF_Block_GetData(blocks[i]);
						memcpy(&min_rec, data+sizeof(int)+recs[i]*sizeof(Record), sizeof(Record));
						pos=i;
						break;
					}
					i++;
				}
				for(i=1; i<min; i++){
					if(bl_id[i]<=(j*bufferSize*(bufferSize-1) + (i+1)*bufferSize)* power(bufferSize-1,m) && bl_id[i]<=sunolika_block){
						data = BF_Block_GetData(blocks[i]);
						memcpy(&cur_rec, data+sizeof(int)+recs[i]*sizeof(Record), sizeof(Record));
						if(fieldNo==0){
							if(cur_rec.id<min_rec.id){
								min_rec=cur_rec;
								pos=i;
							}
						}
						else if(fieldNo==1){
							ret=strcmp(min_rec.name, cur_rec.name);
							if(ret>0){
								min_rec=cur_rec;
								pos=i;
							}
						}
						else if(fieldNo==2){
							ret=strcmp(min_rec.surname, cur_rec.surname);
							if(ret>0){
								min_rec=cur_rec;
								pos=i;
							}
						}
					}
				}
				data = BF_Block_GetData(blocks[pos]);
				memcpy(&counter, data, sizeof(int));
				if(recs[pos]==counter){
					recs[pos]=0;
					if(BF_UnpinBlock(blocks[pos]) != BF_OK){
						return SR_ERROR;
					}
					bl_id[pos]++;
					if(bl_id[pos]<=(j*bufferSize*(bufferSize-1) + (pos+1)*bufferSize)* power(bufferSize-1,m) && bl_id[pos]<=sunolika_block){
						all_blocks--;
						if(flag==1){
							if(BF_GetBlock(fileDesc_temp1, bl_id[pos], blocks[pos]) != BF_OK){
								return SR_ERROR;
							}
						}
						else if(flag==2){
							if(BF_GetBlock(fileDesc_temp2, bl_id[pos], blocks[pos]) != BF_OK){
								return SR_ERROR;
							}
						}	
					}
					else{
						bl_count--;
					}
				}
				else{
					recs[pos]++;					
				}
				data = BF_Block_GetData(block_output);													//Vazoume tin eggrafi sto temp
				memcpy(data+sizeof(int)+recs[bufferSize-1]*sizeof(Record), &min_rec, sizeof(Record));
				recs[bufferSize-1]++;
				memcpy(data, &recs[bufferSize-1], sizeof(int));
				if(recs[bufferSize-1]>=BF_BLOCK_SIZE/sizeof(Record)){
					BF_Block_SetDirty(block_output);
					if(BF_UnpinBlock(block_output) != BF_OK){
						return SR_ERROR;
					}
					recs[bufferSize-1]=0;
					bl_id[bufferSize-1]++;
				}
			}
			if(flag==1){
				flag=2;	
			}
			else if(flag==2){
				flag=1;
			}
		}
		
		if(blocks_num%(bufferSize-1)!=0){
			blocks_num=blocks_num/(bufferSize-1);
			blocks_num++;
		}
		else{
			blocks_num=blocks_num/(bufferSize-1);
		}
		m++;
	}
	free(blocks);
	free(recs);
	free(bl_id);
*/

	if(SR_CreateFile(output_filename) != SR_OK){
		return SR_ERROR;
	}
	if(SR_OpenFile(output_filename, &fileDesc_output) != SR_OK){			// Anoigoume to arxeio pou tha apothikeutei to taxinomimeno
		return SR_ERROR;
	}

	if(flag==1){
		if(BF_GetBlockCounter(fileDesc_temp1, &blocks_num) != BF_OK){
			return SR_ERROR;
		}
	}
	else if(flag==2){
		if(BF_GetBlockCounter(fileDesc_temp2, &blocks_num) != BF_OK){
			return SR_ERROR;
		}	
	}		
	BF_Block_Init(&block_input);
	BF_Block_Init(&block_output);
	for(i=1; i<blocks_num; i++){
		if(flag==1){
			if(BF_GetBlock(fileDesc_temp1, i, block_input) != BF_OK){
				return SR_ERROR;
			}
		}
		else if(flag==2){
			if(BF_GetBlock(fileDesc_temp2, i, block_input) != BF_OK){
				return SR_ERROR;
			}	
		}		
		if(BF_AllocateBlock(fileDesc_output, block_output) != BF_OK){
			return SR_ERROR;
		}
		data = BF_Block_GetData(block_input);
		data_output = BF_Block_GetData(block_output);
		memcpy(&counter, data, sizeof(int));
		memcpy(data_output, &counter, sizeof(int));
		for(j=0; j<counter; j++){
			memcpy(data_output+sizeof(int)+j*sizeof(Record),data+sizeof(int)+j*sizeof(Record), sizeof(Record));
		}
		
		BF_Block_SetDirty(block_output);
		if(BF_UnpinBlock(block_input) != BF_OK){
			return SR_ERROR;
		}
		if(BF_UnpinBlock(block_output) != BF_OK){
			return SR_ERROR;
		}
	}

	if(SR_CloseFile(fileDesc_temp1) != SR_OK){			// Kleinoume kai diagrafoume to temp
			return SR_ERROR;
		}
	if(fieldNo==0){
		if(remove("temp_id1.db")!=0){
			return SR_ERROR;
		}
		if(remove("temp_id2.db")!=0){
			return SR_ERROR;
		}
	}
	else if(fieldNo==1){
		if(remove("temp_name1.db")!=0){
			return SR_ERROR;
		}
		if(remove("temp_name2.db")!=0){
			return SR_ERROR;
		}
	}
	else if(fieldNo==2){
		if(remove("temp_surname1.db")!=0){
			return SR_ERROR;
		}
		if(remove("temp_surname2.db")!=0){
			return SR_ERROR;
		}
	}
  	return SR_OK;
}

SR_ErrorCode SR_PrintAllEntries(int fileDesc) {
  	int block_num,m,i,j,recs;
	char *data;
	BF_Block *block;
	Record record;

	BF_Block_Init(&block);
	
	if(BF_GetBlockCounter(fileDesc, &block_num) != BF_OK){
		return SR_ERROR;
	}
	m=0;
	for(i=1; i<block_num; i++){
		if(BF_GetBlock(fileDesc, i, block) != BF_OK){
			return SR_ERROR;
		}
		data = BF_Block_GetData(block);
		memcpy(&recs, data, sizeof(int));
		for(j=0; j<recs; j++){
			memcpy(&record, data+sizeof(int)+j*sizeof(Record), sizeof(Record));
			m++;
			printf("Record %d:\n\tid:\t%d\n\tname:\t%s\n\tsurname:%s\n\tcity:\t%s\n",m,record.id,record.name,record.surname,record.city);	
		}
		if(BF_UnpinBlock(block) != BF_OK){
			return SR_ERROR;
		}
	}
	
 	return SR_OK;
}
