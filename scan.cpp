#include "scan.h"
#include "byte_read.h"
#include <cstring>


bool ut_a(ulint* type)
{
	if (*type == 1) {
		return true;
	}
	else
	{
		return false;
	}
}

void PrintPageInfo(page_content* page_info)
{
	printf("page_no: %lu, page_type: %u, page_lsn: %lu, page_level:%u, index_id:%lu, records:%u, slots:%u, deleted bytes:%u, last inserted direction:%u\n",
		page_info->page_no,page_info->page_type,page_info->page_lsn,page_info->page_level,page_info->index_id,page_info->records,page_info->slots,
		page_info->deleted_bytes,page_info->direction);
}

void PrintPageStatistics(index_info* info_arrary)
{
	printf("index_id:%u, pages:%lu, index root page no:%u, leaf pages:%lu, left direction pages:%lu, right dirction pages:%lu, no dirction pages:%lu, total deleted bytes:%lu, lsn warnings pages:%lu",
		info_arrary->index_id,info_arrary->root_page,info_arrary->leaf_pages,info_arrary->left_direction_pages,info_arrary->right_dirction_pages,info_arrary->no_dirction_pages,
		info_arrary->total_deleted_bytes,info_arrary->pages_warnings);
}

void ScanPageContent(byte* buffer, page_content* page_info,uint* page_size)
{
	
	page_info->page_no = mach_read_from_4(buffer + FIL_PAGE_OFFSET);

	page_info->page_type = mach_read_from_2(buffer + FIL_PAGE_TYPE);

	page_info->page_lsn = mach_read_from_8(buffer + FIL_PAGE_LSN);

	byte* offset = buffer + FIL_PAGE_DATA;
	page_info->page_level = mach_read_from_2(offset + PAGE_LEVEL);

	page_info->index_id = mach_read_from_8(offset + PAGE_INDEX_ID);

	page_info->records = mach_read_from_2(offset + PAGE_N_RECS);

	page_info->slots = mach_read_from_2(offset + PAGE_N_DIR_SLOTS);

	page_info->deleted_bytes = mach_read_from_2(offset + PAGE_GARBAGE);

	page_info->direction = mach_read_from_2(offset + PAGE_DIRECTION);
	
	uint first_lsn = mach_read_from_4(buffer + FIL_PAGE_LSN + 4);
	uint last_lsn = mach_read_from_4(buffer + (*page_size - 4));

	if (first_lsn == last_lsn)
	{
		page_info->lsn_warngs = 0;
	}
	else
	{
		page_info->lsn_warngs = 1;
	}
}


int ScanPage(FILE* fp,uint* page_size,ulint* pages,ulint* type)
{
	index_info* info_arrary[32];
	uint (*index_id_arrary)[32];
	ulint index_count = 0;

	fseek(fp,(*page_size) * 3 , 0);
	byte *buffer;
	buffer = new byte[*page_size];
	page_content* page_info = new page_content;
	for (ulint i = 4; i <= *pages; i++)
	{
		ulint tmp_index_id = 0; //����id��index_id_arrary�е��±�
		bool is_in_arrary; //�ж��Ƿ��Ѵ��������еı�־

		get_page_value(buffer,fp, page_size);
		ScanPageContent(buffer,page_info,page_size);
		if (ut_a(type))
		{
			is_in_arrary = InArray(index_id_arrary, page_info, &tmp_index_id); //����������Ƿ��Ѵ���������Ϣ
			
			if (!is_in_arrary)  //�������������е���������ָ����Ϣ
			{
				index_info* _info = new index_info;
				_info->index_id = page_info->index_id;
				info_arrary[index_count] = _info;
				tmp_index_id = index_count; //������ָ����Ϣ�������е��±�
				*index_id_arrary[index_count] = page_info->index_id; //��¼index_id�����������ָ�������±���ͬ
				PageStatistics(page_info, info_arrary[index_count]);
				index_count += 1;
			}

		}
		else
		{
			PrintPageInfo(page_info);
		}
		memset(buffer,'\0',*page_size);
	}
	if (ut_a(type)) 
	{
		for (uint i = 0; i < 32; i++)
		{
			PrintPageStatistics(info_arrary[i]);
		}
	}


	delete[]buffer;
	return 1;

}

/*ͳ��������Ϣ*/
void PageStatistics(page_content* page_info, index_info* info_arrary)
{
	if (page_info->page_level == 1) { info_arrary->root_page = page_info->page_no; }
	else if (page_info->page_level == 0){ info_arrary->leaf_pages += 1; }
	info_arrary->total_deleted_bytes += page_info->deleted_bytes;
	info_arrary->total_pages += 1;
	info_arrary->pages_warnings += page_info->lsn_warngs;

	/*���һ�β��뷽��*/
	if (page_info->direction == 1)
	{
		info_arrary->left_direction_pages += 1;
	}
	else if (page_info->direction == 2)
	{
		info_arrary->right_dirction_pages += 1;
	}
	else if (page_info->direction == 5)
	{
		info_arrary->no_dirction_pages += 0;
	}
}

/*����������Ƿ��Ѵ���������Ϣ*/
bool InArray(uint(*index_id_arrary)[32], page_content* page_info,ulint* tmp_index_id)
{
	for (uint i = 0; i < 32; i++) 
	{
		if (*index_id_arrary[i] == page_info->index_id)
		{
			*tmp_index_id = i;
			return true;
		}
		else
		{
			return false;
		}
}
