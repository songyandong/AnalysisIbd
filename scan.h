#include "fsp.h"
#include "pag0page.h"
#include <iostream>


#define byte unsigned char
/*����������Ϣ*/
typedef struct {
	char index_name;
	uint index_id;
	ulint total_pages;
	uint root_page;
	ulint leaf_pages;
}index_info;


typedef struct {
	ulint page_no; //ҳid
	ulint page_type; //ҳ����
	ulint page_lsn; //ҳ��ǰˢ����LSN��
	ulint slots; //ӵ�е�slot����
	uint records; //��¼��
	uint page_level; //��ҳ������������Ҷ�ӽ�㻹��rootҳ��
	uint index_id; //��������ID
	uint direction; //���һ�β���ķ��ģ��������ڹ���ҳ�������
	uint deleted_bytes; //��ҳ��ǰ���ɾ�����ֽ���
}page_content;

int ScanPage(FILE* fp, uint* page_size, ulint* pages);

void PrintPageInfo(page_content* page_info);

void ScanPageContent(byte* buffer);