//// �л����û�ģʽ���С�
// �ú�������iret ָ��ʵ�ִ��ں�ģʽ�л����û�ģʽ����ʼ����0����
#define move_to_user_mode() \
_asm { \
	_asm mov eax,esp /* �����ջָ��esp ��eax �Ĵ����С�*/\
	_asm push 00000017h /* ���Ƚ���ջ��ѡ���(SS)��ջ��*/\
	_asm push eax /* Ȼ�󽫱���Ķ�ջָ��ֵ(esp)��ջ��*/\
	_asm pushfd /* ����־�Ĵ���(eflags)������ջ��*/\
	_asm push 0000000fh /* ���ں˴����ѡ���(cs)��ջ��*/\
	_asm push offset l1 /* ��������l1 ��ƫ�Ƶ�ַ(eip)��ջ��*/\
	_asm iretd /* ִ���жϷ���ָ������ת��������1 ����*/\
_asm l1: mov eax,17h /* ��ʱ��ʼִ������0��*/\
	_asm mov ds,ax /* ��ʼ���μĴ���ָ�򱾾ֲ��������ݶΡ�*/\
	_asm mov es,ax \
	_asm mov fs,ax \
	_asm mov gs,ax \
}
/*
__asm__ ( "movl %%esp,%%eax\n\t" \	
"pushl $0x17\n\t" \		
  "pushl %%eax\n\t" \		
  "pushfl\n\t" \		
  "pushl $0x0f\n\t" \		
  "pushl $1f\n\t" \		
  "iret\n" \			
  "1:\tmovl $0x17,%%eax\n\t" \	
  "movw %%ax,%%ds\n\t" \	// ��ʼ���μĴ���ָ�򱾾ֲ��������ݶΡ�
"movw %%ax,%%es\n\t" "movw %%ax,%%fs\n\t" "movw %%ax,%%gs":::"ax")
*/
#define sti() _asm{ _asm sti }// ���ж�Ƕ����꺯����
//__asm__ ( "sti"::)	
#define cli() _asm{ _asm cli }// ���жϡ�
//__asm__ ( "cli"::)	
#define nop() _asm{ _asm nop }// �ղ�����
//__asm__ ( "nop"::)	
#define iret() _asm{ _asm iretd }// �жϷ��ء�
//__asm__ ( "iret"::)	

//// �������������꺯����
// ������gate_addr -��������ַ��type -��������������ֵ��dpl -��������Ȩ��ֵ��addr -ƫ�Ƶ�ַ��
// %0 - (��dpl,type ��ϳɵ����ͱ�־��)��%1 - (��������4 �ֽڵ�ַ)��
// %2 - (��������4 �ֽڵ�ַ)��%3 - edx(����ƫ�Ƶ�ַaddr)��%4 - eax(�����к��ж�ѡ���)��
void _inline _set_gate(unsigned long *gate_addr, \
					   unsigned short type, \
					   unsigned short dpl, \
					   unsigned long addr) 
{// c���ͻ����䶼����ͨ��
	gate_addr[0] = 0x00080000 + (addr & 0xffff);
	gate_addr[1] = 0x8000 + (dpl << 13) + (type << 8) + (addr & 0xffff0000);
/*	unsigned short tmp = 0x8000 + (dpl << 13) + (type << 8);
	_asm mov eax,00080000h ;
	_asm mov edx,addr ;
	_asm mov ax,dx ;// ��ƫ�Ƶ�ַ�������ѡ�����ϳ���������4 �ֽ�(eax)��
	_asm mov dx,tmp ;// �����ͱ�־����ƫ�Ƹ�����ϳ���������4 �ֽ�(edx)��
	_asm mov ebx,gate_addr
	_asm mov [ebx],eax ;// �ֱ��������������ĵ�4 �ֽں͸�4 �ֽڡ�
	_asm mov [ebx+4],edx ;*/
}
/*
__asm__ ( "movw %%dx,%%ax\n\t" \	
  "movw %0,%%dx\n\t" \		
  "movl %%eax,%1\n\t" \		
"movl %%edx,%2":
:"i" ((short) (0x8000 + (dpl << 13) + (type << 8))),
  "o" (*((char *) (gate_addr))),
  "o" (*(4 + (char *) (gate_addr))), "d" ((char *) (addr)), "a" (0x00080000))
*/

//// �����ж��ź�����
// ������n - �жϺţ�addr - �жϳ���ƫ�Ƶ�ַ��
// &idt[n]��Ӧ�жϺ����ж����������е�ƫ��ֵ���ж���������������14����Ȩ����0��
#define set_intr_gate(n,addr) \
_set_gate((unsigned long*)(&(idt[n])),14,0,(unsigned long)addr)
//// ���������ź�����
// ������n - �жϺţ�addr - �жϳ���ƫ�Ƶ�ַ��
// &idt[n]��Ӧ�жϺ����ж����������е�ƫ��ֵ���ж���������������15����Ȩ����0��
#define set_trap_gate(n,addr) \
_set_gate((unsigned long*)(&(idt[n])),15,0,(unsigned long)addr)
//// ����ϵͳ�����ź�����
// ������n - �жϺţ�addr - �жϳ���ƫ�Ƶ�ַ��
// &idt[n]��Ӧ�жϺ����ж����������е�ƫ��ֵ���ж���������������15����Ȩ����3��
#define set_system_gate(n,addr) \
_set_gate((unsigned long*)(&(idt[n])),15,3,(unsigned long)addr)
//// ���ö�������������
// ������gate_addr -��������ַ��type -��������������ֵ��dpl -��������Ȩ��ֵ��
// base - �εĻ���ַ��limit - ���޳������μ����������ĸ�ʽ��
#define _set_seg_desc(gate_addr,type,dpl,base,limit) {\
*(gate_addr) = ((base) & 0xff000000) | \
				(((base) & 0x00ff0000) >> 16) | \
				((limit) & 0xf0000) | \
				((dpl) << 13) | \
				(0x00408000) | \
				((type) << 8);/* ��������4 �ֽڡ�*/\
*((gate_addr) + 1) = (((base) & 0x0000ffff) << 16) | \
				((limit) & 0x0ffff);/* ��������4 �ֽڡ�*/ \
}

//// ��ȫ�ֱ�����������״̬��/�ֲ�����������
// ������n - ��ȫ�ֱ�����������n ����Ӧ�ĵ�ַ��addr - ״̬��/�ֲ��������ڴ�Ļ���ַ��
// tp - �������еı�־�����ֽڡ�
// %0 - eax(��ַaddr)��%1 - (��������n �ĵ�ַ)��%2 - (��������n �ĵ�ַƫ��2 ��)��
// %3 - (��������n �ĵ�ַƫ��4 ��)��%4 - (��������n �ĵ�ַƫ��5 ��)��
// %5 - (��������n �ĵ�ַƫ��6 ��)��%6 - (��������n �ĵ�ַƫ��7 ��)��
extern _inline void _set_tssldt_desc(unsigned short *n,unsigned long addr,char tp) 
{ 
/*	n[0] = 104;
	n[1] = addr;
	n[2] = addr >> 16;
	((char*)n)[7] = ((char*)n)[5];
	((char*)n)[5] = tp;
	((char*)n)[6] = 0;*/
	_asm mov ebx,n
	_asm mov ax,104 
	_asm mov word ptr [ebx],ax // ��TSS ���ȷ���������������(��0-1 �ֽ�)��
	_asm mov eax,addr 
	_asm mov word ptr [ebx+2],ax // ������ַ�ĵ��ַ�����������2-3 �ֽڡ�
	_asm ror eax,16 // ������ַ��������ax �С�
	_asm mov byte ptr [ebx+4],al // ������ַ�����е��ֽ�������������4 �ֽڡ�
	_asm mov al,tp
	_asm mov byte ptr [ebx+5],al // ����־�����ֽ������������ĵ�5 �ֽڡ�
	_asm mov al,0 
	_asm mov byte ptr [ebx+6],al // �������ĵ�6 �ֽ���0��
	_asm mov byte ptr [ebx+7],ah // ������ַ�����и��ֽ�������������7 �ֽڡ�
	_asm ror eax,16 // eax ���㡣
}
/*
__asm__ ( "movw $104,%1\n\t" \	
"movw %%ax,%2\n\t" \		
  "rorl $16,%%eax\n\t" \	
  "movb %%al,%3\n\t" \		
  "movb $" type ",%4\n\t" \	
  "movb $0x00,%5\n\t" \		
  "movb %%ah,%6\n\t" \		
  "rorl $16,%%eax" \		
  ::"a" (addr), "m" (*(n)), "m" (*(n + 2)), "m" (*(n + 4)),
  "m" (*(n + 5)), "m" (*(n + 6)), "m" (*(n + 7)))
*/
//// ��ȫ�ֱ�����������״̬����������
// n - �Ǹ���������ָ�룻addr - ���������еĻ���ֵַ������״̬����������������0x89��
#define set_tss_desc(n,addr) \
_set_tssldt_desc((unsigned short*)(n),(unsigned long)(addr), (char)0x89)
//// ��ȫ�ֱ������þֲ�����������
// n - �Ǹ���������ָ�룻addr - ���������еĻ���ֵַ���ֲ�����������������0x82��
#define set_ldt_desc(n,addr) \
_set_tssldt_desc((unsigned short*)(n),(unsigned long)(addr), (char)0x82)