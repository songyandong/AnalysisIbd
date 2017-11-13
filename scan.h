#include "fsp.h"
#include "pag0page.h"
#include <iostream>


#define byte unsigned char
/*����������Ϣ*/
typedef struct {
	char index_name;
	uint index_id;
	ulint total_pages = 0;
	uint root_page;
	ulint leaf_pages;
	ulint left_direction_pages;
	ulint right_dirction_pages; 
	ulint no_dirction_pages;
	ib_uint64_t total_deleted_bytes; //����ҳ��ɾ���ֽ�ͳ��
	ulint pages_warnings; //ҳlsn�쳣ͳ��
}index_info;


typedef struct {
	ulint page_no; //ҳid
	ulint page_type; //ҳ����
	ulint page_lsn; //ҳ��ǰˢ����LSN��
	uint lsn_warngs;
	ulint slots; //ӵ�е�slot����
	uint records; //��¼��
	uint page_level; //��ҳ������������Ҷ�ӽ�㻹��rootҳ��
	uint index_id; //��������ID
	uint direction; //���һ�β���ķ��ģ��������ڹ���ҳ�������
	uint deleted_bytes; //��ҳ��ǰ���ɾ�����ֽ���
}page_content;

int ScanPage(FILE* fp, uint* page_size, ulint* pages,ulint* type);

void PrintPageInfo(page_content* page_info);

void ScanPageContent(byte* buffer, page_content* page_info,uint* page_size);

void PageStatistics(page_content* page_info, index_info* info_arrary);

bool InArray(uint (*index_id_arrary)[32], page_content* page_info, ulint* tmp_index_id,uint* index_count);