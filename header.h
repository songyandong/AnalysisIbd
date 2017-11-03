#include "fsp.h"
#include "pag0page.h"


struct point_info
{
	uint addr_page;
	uint addr_offset;
};

struct fsp_info
{
	uint flst_len;  //������
	point_info flst_first; //�����һ����ָ��
	point_info flst_last; //������������һ����ָ��
};


struct Fsp_Info
{
	FILE *fp;
	uint page_ssize;
	uint space_id; //��ռ�id
	uint page_size; //ҳ��С
	uint free_len;
	uint free_limit;
	bool is_com;
	uint page_number;
	uint fil_page_type; //����
	uint fsp_space_id; //��ռ�id
	uint fsp_not_used; //δʹ�õĴ�����
	uint fsp_size; //��ռ���ҳ��
	uint fsp_space_flags; //flags ��λ���space_id��ѹ��ҳ��С,ѹ�����͵�
	uint fsp_frag_n_used; //FSP_FREE_FRAG���Ѿ���ʹ�õ�ҳ����
	fsp_info fsp_free; //ƫ��FSP_FREE + FLST_LEN,д��0ռ��4bytes,ƫ��FSP_FREE + FLST_FIRST/FLST_LAST + FIL_ADDR_PAGE��4bytes��¼ָ����Ϣ
					   //FSP_FREE + FLST_FIRST/FLST_LAST + FIL_ADDR_BYTEռ��2bytes��offset��Ϣ�� ��¼��ռ������п��дص�������Ϣ
	fsp_info fsp_free_frag;//��FSP_FREE���ƣ�ֻ�����Ǽ�¼�Ĵ�Ϊ�Ѿ�����ʹ�õģ������ǿմ��ֲ������ص�����
	fsp_info fsp_full_frag;//ͬ�ϣ�������˼������洢��Ϊ����
	fsp_info fsp_seg_inodes_full;//ͬ�ϣ����ڴ�Ź������ݶε�ҳ��������ָ��Ϊ����������ҳ����
	fsp_info fsp_seg_inodes_free;//ͬ�ϣ����ָ��Ϊ����������ݶι�����Ϣ������ҳ����
	uint fsp_seg_id;//ƫ��FSP_SEG_ID��¼��ID��ռ��1bytes�����ݶ�ID
};

//��ȡpage����
char* get_page_value(FILE* fp, uint* page_size);

void Print_content(Fsp_Info *fsp_info_value);

//��ȡpage_size
int page_size_t(uint fsp_flags);

//convert the byte of Big-Endian and Little-Endian
int conversion_byte_order(int num);

int check_unside(uint value);

int conversion_byte_order_two(short num);

//��ȡ4bytes
int read_int(FILE *file);

//��ȡ�ء��ε�ָ����Ϣ
void *read_fsp_content(Fsp_Info *fsp_info_value, char* buffer, uint offset, int type);

void Enter(char *file_name);

void Print_content(Fsp_Info *fsp_info_value);

//��ȡҳ����
char* get_page_value(FILE* fp, uint* page_size);
