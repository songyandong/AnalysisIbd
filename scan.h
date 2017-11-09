#include "fsp.h"
#include "pag0page.h"
#include <iostream>


#define uchar unsigned char
/*索引基本信息*/
typedef struct {
	char index_name;
	uint index_id;
	ulint total_pages;
	uint root_page;
	ulint leaf_pages;
}index_info;


typedef struct {
	uchar page_no;
	uchar page_type;
	uchar page_lsn;
	uchar slots;
	char page_direction;
	uint records;
	uint page_level;
	uint index_id;
}page_content;

int ScanPage(FILE* fp, uint* page_size, ulint* pages);

void PrintPageInfo(page_content* page_info);

void ScanPageContent(char* buffer);