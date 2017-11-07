//����1���Զ�����ģʽ�򿪲�д���ļ�
#include <stdio.h>
#include <stdlib.h>  
#include <iostream>
#include <cstring>
#include "header.h"


using namespace std;







//��ȡpage_size
int page_size_t(uint fsp_flags)
{
	uint	ssize = FSP_FLAGS_GET_PAGE_SSIZE(fsp_flags);

	/* If the logical page size is zero in fsp_flags, then use the
	legacy 16k page size. */
	ssize = (0 == ssize) ? UNIV_PAGE_SSIZE_ORIG : ssize;

	/* Convert from a 'log2 minus 9' to a page size in bytes. */
	const uint	size = ((UNIV_ZIP_SIZE_MIN >> 1) << ssize);

	return size;
}



//�Ƿ�Ϊ-1
int check_unside(uint value)
{
	if (value == -1)
	{
		return 0;
	}
	else
		return value;
}



//��ȡ4bytes
int read_int(FILE *file)
{
	int value;
	fread(&value, sizeof(int), 1, file);
	return value;
}

//��ȡ�ء��ε�ָ����Ϣ
void *read_fsp_content(Fsp_Info *fsp_info_value, char* buffer, uint offset, int type)
{
	ulint flst_len = check_unside(mach_read_from_4(buffer + offset + FLST_LEN));
	ulint flst_first_page = check_unside(mach_read_from_4(buffer + offset + FLST_FIRST + FIL_ADDR_PAGE));
	ulint flst_first_page_offset = check_unside(mach_read_from_2(buffer + offset + FLST_FIRST + FIL_ADDR_BYTE));
	ulint flst_last_page = check_unside(mach_read_from_4(buffer + offset + FLST_LAST + FIL_ADDR_PAGE));
	ulint flst_last_page_offset = check_unside(mach_read_from_2(buffer + offset + FLST_LAST + FIL_ADDR_BYTE));

	if (type == 1)
	{
		fsp_info_value->fsp_free.flst_len = flst_len;
		fsp_info_value->fsp_free.flst_first.addr_page = flst_first_page;
		fsp_info_value->fsp_free.flst_first.addr_offset = flst_first_page_offset;
		fsp_info_value->fsp_free.flst_last.addr_page = flst_last_page;
		fsp_info_value->fsp_free.flst_last.addr_offset = flst_last_page_offset;
	}
	else if (type == 2)
	{
		fsp_info_value->fsp_free_frag.flst_len = flst_len;
		fsp_info_value->fsp_free_frag.flst_first.addr_page = flst_first_page;
		fsp_info_value->fsp_free_frag.flst_first.addr_offset = flst_first_page_offset;
		fsp_info_value->fsp_free_frag.flst_last.addr_page = flst_last_page;
		fsp_info_value->fsp_free_frag.flst_last.addr_offset = flst_last_page_offset;
	}
	else if (type == 3)
	{
		fsp_info_value->fsp_full_frag.flst_len = flst_len;
		fsp_info_value->fsp_full_frag.flst_first.addr_page = flst_first_page;
		fsp_info_value->fsp_full_frag.flst_first.addr_offset = flst_first_page_offset;
		fsp_info_value->fsp_full_frag.flst_last.addr_page = flst_last_page;
		fsp_info_value->fsp_full_frag.flst_last.addr_offset = flst_last_page_offset;
	}
	else if (type == 4)
	{
		fsp_info_value->fsp_seg_inodes_free.flst_len = flst_len;
		fsp_info_value->fsp_seg_inodes_free.flst_first.addr_page = flst_first_page;
		fsp_info_value->fsp_seg_inodes_free.flst_first.addr_offset = flst_first_page_offset;
		fsp_info_value->fsp_seg_inodes_free.flst_last.addr_page = flst_last_page;
		fsp_info_value->fsp_seg_inodes_free.flst_last.addr_offset = flst_last_page_offset;
	}
	else if (type == 5)
	{
		fsp_info_value->fsp_seg_inodes_full.flst_len = flst_len;
		fsp_info_value->fsp_seg_inodes_full.flst_first.addr_page = flst_first_page;
		fsp_info_value->fsp_seg_inodes_full.flst_first.addr_offset = flst_first_page_offset;
		fsp_info_value->fsp_seg_inodes_full.flst_last.addr_page = flst_last_page;
		fsp_info_value->fsp_seg_inodes_full.flst_last.addr_offset = flst_last_page_offset;
	}

}




char* get_page_value(FILE* fp,uint* page_size)
{
	char *page;
	page = new char[*page_size];
	fread(page, *page_size, 1, fp);
	return page;
}


void Enter(char *file_name)
{
	Fsp_Info *fsp_info_value = new Fsp_Info;
	FILE *fp = fsp_info_value->fp = fopen(file_name, "rb+");   //������ģʽ
	if (NULL == fp)
	{
		cout << file_name << ": No such file or directory" << endl;
		exit;
	}

	uint tmp_value;
	//page_size
	fseek(fp, FSP_HEADER_OFFSET + FSP_SPACE_FLAGS,0);
	fread(&tmp_value, 4, 1, fp);
	fsp_info_value->page_size = page_size_t(tmp_value);
	

	/* ��headerҳ����buffer*/
	char *buffer;
	buffer = new char[fsp_info_value->page_size];
	fseek(fp, 0, 0);
	fread(buffer, fsp_info_value->page_size, 1, fp);


	//page_ssize
	fsp_info_value->page_ssize = FSP_FLAGS_GET_PAGE_SSIZE(mach_little_read_from_4(buffer + FSP_HEADER_OFFSET + FSP_SPACE_FLAGS));

	//page_number
	fsp_info_value->page_number = mach_read_from_4(buffer + FIL_PAGE_OFFSET);

	//size_in_header
	fsp_info_value->fsp_size = mach_read_from_4(buffer + FSP_HEADER_OFFSET + FSP_SIZE);

	//space_id
	fsp_info_value->space_id = mach_read_from_4(buffer + FSP_HEADER_OFFSET);

	//id
	fsp_info_value->fsp_space_id = mach_read_from_4(buffer + FIL_PAGE_ARCH_LOG_NO_OR_SPACE_ID);

	//free_limit
	fsp_info_value->free_limit = mach_read_from_4(buffer + FSP_HEADER_OFFSET + FSP_FREE_LIMIT);

	//free_len
	fsp_info_value->free_len = mach_read_from_4(buffer + FSP_HEADER_OFFSET + FSP_FREE + FLST_LEN);

	//page_type_fsp_hdr
	fsp_info_value->fil_page_type = mach_read_from_2(buffer + FIL_PAGE_TYPE);

	//fsp_not_used
	fsp_info_value->fsp_not_used = mach_read_from_4(buffer + FSP_HEADER_OFFSET + FSP_NOT_USED);

	//fsp_flags
	fsp_info_value->fsp_space_flags = mach_little_read_from_4(buffer + FSP_HEADER_OFFSET + FSP_SPACE_FLAGS);

	//fsp_frag_n_used
	fsp_info_value->fsp_frag_n_used = mach_read_from_4(buffer + FSP_HEADER_OFFSET + FSP_FRAG_N_USED);

	//is_com �Ƿ�ѹ��
	fsp_info_value->is_com = page_is_comp(buffer);

	/*���ڶ�ָ��*/
	//���д���Ϣ
	read_fsp_content(fsp_info_value, buffer, FSP_HEADER_OFFSET + FSP_FREE, 1);
	//��մ���Ϣ
	read_fsp_content(fsp_info_value, buffer, FSP_HEADER_OFFSET + FSP_FREE_FRAG, 2);
	//������Ϣ
	read_fsp_content(fsp_info_value, buffer, FSP_HEADER_OFFSET + FSP_FULL_FRAG, 3);

	//���ݶ���Ϣ
	//����������ҳ������Ϣ
	read_fsp_content(fsp_info_value, buffer, FSP_HEADER_OFFSET + FSP_SEG_INODES_FULL, 4);
	//����������ҳ������Ϣ
	read_fsp_content(fsp_info_value, buffer, FSP_HEADER_OFFSET + FSP_SEG_INODES_FREE, 5);


	delete[]buffer;
	fclose(fp);
	Print_content(fsp_info_value);
	delete[]fsp_info_value;
}

void Print_content(Fsp_Info *fsp_info_value)
{
	printf("INFO: space_id: %d, is_compact: %s, page_ssize: %d, page_size: %d, pages_used: %d ,page_number: %d\n", fsp_info_value->space_id,
		fsp_info_value->is_com, fsp_info_value->page_ssize, fsp_info_value->page_size, fsp_info_value->fsp_size,fsp_info_value->page_number);

	printf("FSP �� fsp_flags: %d, fsp_not_used: %d, fsp_frag_n_used: %d\n", fsp_info_value->fsp_space_flags, fsp_info_value->fsp_not_used,
		fsp_info_value->fsp_frag_n_used);

	printf("     fsp_free: len: %d, firset_page_number: %d, firset_page_offset: %d, last_page_number: %d, last_page_offset: %d\n",
		fsp_info_value->fsp_free.flst_len, fsp_info_value->fsp_free.flst_first.addr_page, fsp_info_value->fsp_free.flst_first.addr_offset,
		fsp_info_value->fsp_free.flst_last.addr_page, fsp_info_value->fsp_free.flst_last.addr_offset);

	printf("     fsp_free_frag: len: %d, firset_page_number: %d, firset_page_offset: %d, last_page_number: %d, last_page_offset: %d\n",
		fsp_info_value->fsp_free_frag.flst_len, fsp_info_value->fsp_free_frag.flst_first.addr_page, fsp_info_value->fsp_free_frag.flst_first.addr_offset,
		fsp_info_value->fsp_free_frag.flst_last.addr_page, fsp_info_value->fsp_free_frag.flst_last.addr_offset);

	printf("     fsp_full_frag: len: %d, firset_page_number: %d, firset_page_offset: %d, last_page_number: %d, last_page_offset: %d\n",
		fsp_info_value->fsp_full_frag.flst_len, fsp_info_value->fsp_full_frag.flst_first.addr_page, fsp_info_value->fsp_full_frag.flst_first.addr_offset,
		fsp_info_value->fsp_full_frag.flst_last.addr_page, fsp_info_value->fsp_full_frag.flst_last.addr_offset);


	printf("FSP_SEG: \n");
	printf("       fsp_seg_free: len: %d, firset_page_number: %d, firset_page_offset: %d, last_page_number: %d, last_page_offset: %d\n",
		fsp_info_value->fsp_seg_inodes_free.flst_len, fsp_info_value->fsp_seg_inodes_free.flst_first.addr_page, fsp_info_value->fsp_seg_inodes_free.flst_first.addr_offset,
		fsp_info_value->fsp_seg_inodes_free.flst_last.addr_page, fsp_info_value->fsp_seg_inodes_free.flst_last.addr_offset);

	printf("       fsp_seg_full: len: %d, firset_page_number: %d, firset_page_offset: %d, last_page_number: %d, last_page_offset: %d\n",
		fsp_info_value->fsp_seg_inodes_full.flst_len, fsp_info_value->fsp_seg_inodes_full.flst_first.addr_page, fsp_info_value->fsp_seg_inodes_full.flst_first.addr_offset,
		fsp_info_value->fsp_seg_inodes_full.flst_last.addr_page, fsp_info_value->fsp_seg_inodes_full.flst_last.addr_offset);

}

