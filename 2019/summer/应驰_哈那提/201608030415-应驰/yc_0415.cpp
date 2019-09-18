#include<iostream>
#include<stdint.h>
using namespace std;

//������ 
#define LUI 	0x37 
#define ALUPC 	0x17
#define JAL 	0x6f
#define JALR 	0x67
#define BType  	0x63 //B��ָ��
#define LType	0x03 //L��ָ�� 
#define SType	0x23 //S��ָ��
#define IType	0x13 //I��ָ�� 
#define RType	0x33 //R��ָ�� 

//B��ָ���֧������
#define BEQ 	0
#define BNE 	1
#define BLT 	4
#define BGE 	5
#define BLTU 	6
#define BGEU 	7

//L��ָ���֧������ 
#define LB		0
#define LH		1
#define LW		2
#define LBU		4
#define LHU		5

//S��ָ���֧������
#define SB		0
#define SH		1
#define SW		2

//I��ָ���֧������ 
#define ADDI	0
#define	SLTI	2
#define	SLTIU	3
#define	XORI	4
#define	ORI		6
#define	ANDI	7	 
#define SLLI	1
#define SRL_AI	5//SRLI��SRAI������ͬ������

#define SRLI	0
#define SRAI	0x20 

//R��ָ���֧������
#define ADD_SUB 0
#define SLL		1
#define SLT		2		 
#define SLTU	3
#define XOR		4
#define SRL_SRA	5
#define OR		6
#define AND		7

#define ADD		0
#define SUB		0x20
#define SRL		0
#define SRA		0x20
 
//�洢������ 
uint32_t Msize=0;
const int wordsize = sizeof(uint32_t); 
char* M;  

//�����ڴ�ռ�
int allocMem(uint32_t s){
	M=new char[s];
	Msize=s;
	return s;
} 
//�ͷ��ڴ�ռ� 
void freeMem(){
	delete[] M;
	Msize=0;
}

int32_t ReadByte(uint32_t addr,bool flag){ //�Ӵ洢����8λֵ 
	if(addr>=Msize) {
		cout<<"ERROR:��ַ��Χ�����ڴ�����"<<endl;
		return 0; 
	}
	if(flag==1) //�����з���
		return M[addr];
	else 
		return (unsigned char)M[addr];
} 

void WriteByte(uint32_t addr,char data){ //д8λֵ��洢�� 
	if(addr>=Msize) {
		cout<<"ERROR:��ַ��Χ�����ڴ�����"<<endl;
		return; 
	}
	M[addr]=data;
} 

int32_t Read2Byte(uint32_t addr,bool flag){ //�Ӵ洢����16λֵ
	if(addr>=Msize-wordsize/2) {
		cout<<"ERROR:��ַ��Χ�����ڴ�����"<<endl;
		return 0; 
	}
	if (flag==1) //�����з��ŵ�
		return *((int16_t*)&(M[addr]));
	else
		return *((uint16_t*)&(M[addr]));
} 

void Write2Byte(uint32_t addr,uint32_t data){ //дֵ��洢�� 
	if(addr>=Msize-wordsize/2) {
		cout<<"ERROR:��ַ��Χ�����ڴ�����"<<endl;
		return; 
	}
	*((uint16_t*)&(M[addr]))=data;
}
int32_t ReadWord(uint32_t addr){ //�Ӵ洢����32λֵ
	if(addr>=Msize-wordsize) {
		cout<<"ERROR:��ַ��Χ�����ڴ�����"<<endl;
		return 0; 
	}
	return *((int32_t*)&(M[addr]));
} 

void WriteWord(uint32_t addr,uint32_t data){ //д32λֵ��洢�� 
	if(addr>=Msize-wordsize) {
		cout<<"ERROR:��ַ��Χ�����ڴ�����"<<endl;
		return; 
	}
	*((uint32_t*)&(M[addr]))=data;
}

uint32_t R[33]; 
uint32_t PC,nextPC,IR; 

unsigned int opcode;
unsigned int r1,r2,rd;

//��֧������
unsigned int func3; 
unsigned int func7; 

unsigned int imm31_12U;
unsigned int imm31_12U_0;

//JALָ���ʽ����λ��� ,����λ���� 
unsigned int imm31J;//����λ
unsigned int imm30_21J;
unsigned int imm20J;
unsigned int imm19_12J;
unsigned int imm_sign_31_12J;

//JALR ָ��������ǰ12λ
unsigned int imm31_20JR;

//B��ָ��,����λ 
unsigned int imm31B;//��imm31J���
unsigned int imm30_25B;
unsigned int imm11_8B;
unsigned int imm7B;
unsigned int imm_sign_31_25B_11_7B;

//L��ָ�����λ 
unsigned int imm31_20L;

//S��ָ�����λ
unsigned int imm31S;//��imm31J���
unsigned int imm30_25S;
unsigned int imm11_7S;
unsigned int imm_sign_31_25S_11_7S; 

//I��ָ�����λ
int imm_sign_31_20I;
unsigned int shamt; 
 

void Program(){
	/*U��ָ��*/ 
	WriteWord(0,(0x12345<<12)|(1<<7)|(LUI));
	WriteWord(4,(0x2<<12)|(2<<7)|(ALUPC));
	/*J��ָ��*/
	WriteWord(8,(0<<31)|(4<<21)|(0<<20)|(0<<12)|(3<<7)|(JAL)); 
	WriteWord(16,(12<<20)|(5<<15)|(0<<12)|(4<<7)|(JALR));

	/*B��ָ��*/
	//BEQ��r5=r6=0,��ת����������=4��PC=24+4*2=32;
	WriteWord(24,(0<<31)|(0<<25)|(6<<20)|(5<<15)|(0<<12)|(4<<8)|(0<<7)|(BType)); 
	//BNE��r3!=r6,��ת����������=6��PC=32+6*2=44;
	WriteWord(32,(0<<31)|(0<<25)|(6<<20)|(3<<15)|(1<<12)|(6<<8)|(0<<7)|(BType));
	//BLT��r6<r3,��ת����������=4��PC=44+4*2=52;
	WriteWord(44,(0<<31)|(0<<25)|(3<<20)|(6<<15)|(4<<12)|(4<<8)|(0<<7)|(BType));
	//BGE��r3>r6,��ת����������=4��PC=52+4*2=60;
	WriteWord(52,(0<<31)|(0<<25)|(6<<20)|(3<<15)|(5<<12)|(4<<8)|(0<<7)|(BType));
	//BLTU��r6<r3,��ת����������=4��PC=60+4*2=68;
	WriteWord(60,(0<<31)|(0<<25)|(3<<20)|(6<<15)|(6<<12)|(4<<8)|(0<<7)|(BType));
	//BGEU��r3>r6,��ת����������=4��PC=68+4*2=76;
	WriteWord(68,(0<<31)|(0<<25)|(6<<20)|(3<<15)|(7<<12)|(4<<8)|(0<<7)|(BType));
	
	/*L��ָ��*/
	M[1024]=0xfe;
	M[1025]=0xf6; 
	M[1026]=0x34;
	M[1027]=0x12;
	//LB����M[r1+imm]�е��ֽڷ�����չ32λ����rd5��,��r3=12+1012=1024 
	WriteWord(76,(1012<<20)|(3<<15)|(0<<12)|(5<<7)|(LType)); 
	//LH����M[r1+imm]�����µ�2�ֽڷ�����չ32λ����rd6��,��r3=12+1012=1024 
	WriteWord(80,(1012<<20)|(3<<15)|(1<<12)|(6<<7)|(LType)); 
	//LH����M[r1+imm]�����µ�4�ֽڷ���rd7��,��r3=12+1012=1024 
	WriteWord(84,(1012<<20)|(3<<15)|(2<<12)|(7<<7)|(LType)); 
	//LBU����M[r1+imm]�е��ֽ��޷�����չ32λ����rd8��,��r3=12+1012=1024 
	WriteWord(88,(1012<<20)|(3<<15)|(4<<12)|(8<<7)|(LType)); 
	//LHU����M[r1+imm]�����µ�2�ֽ��޷�����չ32λ����rd9��,��r3=12+1012=1024 
	WriteWord(92,(1012<<20)|(3<<15)|(5<<12)|(9<<7)|(LType)); 
	
	/*S��ָ�� */
	//SB:��r2�ĵ�8λ�����ַΪr1+�������з�����չ�Ĵ洢����
	/*���Դ洢����ַΪ���������Ĵ���r11=0,*/ 
	WriteWord(96,(0x10<<25)|(7<<20)|(11<<15)|(0<<12)|(0<7)|(SType)); 
	WriteWord(100,(0x10<<25)|(7<<20)|(11<<15)|(1<<12)|(0<7)|(SType));
	WriteWord(104,(0x10<<25)|(7<<20)|(11<<15)|(2<<12)|(0<7)|(SType));
	
	/*I��ָ��*/
	WriteWord(108,(0xfff<<20)|(3<<15)|(0<<12)|(10<<7)|(IType)); 
	WriteWord(112,(0xfff<<20)|(3<<15)|(2<<12)|(11<<7)|(IType)); 
	WriteWord(116,(0xfff<<20)|(3<<15)|(3<<12)|(12<<7)|(IType)); 
	WriteWord(120,(0xff3<<20)|(7<<15)|(4<<12)|(13<<7)|(IType)); 
	WriteWord(124,(0xf01<<20)|(2<<15)|(6<<12)|(14<<7)|(IType));	
	WriteWord(128,(0xf4<<20)|(2<<15)|(7<<12)|(15<<7)|(IType));   
	
	//��λ
	WriteWord(132,(0<<25)|(4<<20)|(6<<15)|(1<<12)|(16<<7)|(IType));//���� 
	WriteWord(136,(0<<25)|(4<<20)|(6<<15)|(5<<12)|(17<<7)|(IType));//�߼����� 
	WriteWord(140,(0x20<<25)|(4<<20)|(6<<15)|(5<<12)|(18<<7)|(IType));//�߼����� 
	
	/*R��ָ��*/
	WriteWord(144,(0<<25)|(10<<20)|(3<<15)|(0<<12)|(19<<7)|(RType));
	WriteWord(148,(0x20<<25)|(3<<20)|(10<<15)|(0<<12)|(20<<7)|(RType));
	WriteWord(152,(0<<25)|(6<<20)|(7<<15)|(1<<12)|(21<<7)|(RType)); 
	WriteWord(156,(0<<25)|(7<<20)|(5<<15)|(2<<12)|(22<<7)|(RType)); 
	WriteWord(160,(0<<25)|(5<<20)|(7<<15)|(3<<12)|(23<<7)|(RType)); 
	
	WriteWord(164,(0<<25)|(3<<20)|(4<<15)|(4<<12)|(24<<7)|(RType)); 
	WriteWord(168,(0<<25)|(6<<20)|(21<<15)|(5<<12)|(25<<7)|(RType));
	WriteWord(172,(0x20<<25)|(6<<20)|(21<<15)|(5<<12)|(26<<7)|(RType));
	WriteWord(176,(0<<25)|(1<<20)|(2<<15)|(6<<12)|(27<<7)|(RType)); 
	WriteWord(180,(0<<25)|(1<<20)|(17<<15)|(7<<12)|(28<<7)|(RType)); 
} 
void Decode(unsigned int IR){ //ָ������ 
	opcode= IR & 0x7f;
	rd= (IR>>7)& 0x1f;
	r1=	(IR>>15)&0x1f;
	r2= (IR>>20)&0x1f;
	func3=(IR>>12)&0x7;
	func7=(IR>>25)&0x7f; 
	
	imm31_12U = (IR>>12)& 0xfffff;//ȡ���޷��ŵ�ǰ20λ(U��) 
	
	imm31J=(IR>>31) & 1; //ȡ������λ 
	imm30_21J=(IR>>21) & 0x3ff;
	imm20J=(IR>>20) &1;
	imm19_12J=(IR>>12) & 0xff; 
	
	imm31_20JR=IR>>20;
	/*B��ָ��*/ 
	imm31B=imm31J;
	imm30_25B=(IR>>25)& 0x3f;
	imm11_8B=(IR>>8)&0xf;
	imm7B=(IR>>7)&0x1;
	/*L��ָ��*/
	imm31_20L=IR>>20; 
	/*S��ָ��*/
	imm31S=imm31J;
	imm30_25S=(IR>>25)&0x3f;
	imm11_7S=(IR>>7) & 0x1f;
	/*I��ָ��*/
	imm_sign_31_20I=(int)IR>>20;
	//cout<<imm_sign_31_20I<<"hhhimm"<<endl; 
	shamt=(IR>>20)&0x1f;
	 
	/***********����ƴ��*************/ 
	imm31_12U_0 = imm31_12U<<12;//��0����12λ������U��ָ�� 
	imm_sign_31_12J=(imm31J<<20)&0xfff0000|(imm19_12J<<12)|(imm20J<<11)|(imm30_21J);//JAL
	imm_sign_31_25B_11_7B=(imm31B<<12)&0xffff000|(imm7B<<11)|(imm30_25B<<5)|(imm11_8B);
	imm_sign_31_25S_11_7S=(imm31S<<12)&0xffff000|(imm30_25S<<5)|imm11_7S; //S���з�����չ  
	
} 

void showRegs(){//hex , oct�����ƣ�decʮ���� 
	cout<<"PC=0x"<<hex<<PC<<"  "<<"IR=0x"<<IR<<endl; 
	cout<<"32���Ĵ���ֵ(16����)�ֱ�Ϊ��"<<endl;
	for(int i=1;i<9;i++){
		cout<<"R["<<dec<<i<<"]="<<hex<<R[i]<<'\t'; 
	}
	cout<<endl;
	
	for(int i=9;i<17;i++){
		cout<<"R["<<dec<<i<<"]="<<hex<<R[i]<<'\t'; 
	} 
	cout<<endl;
	
	for(int i=17;i<25;i++){
		cout<<"R["<<dec<<i<<"]="<<hex<<R[i]<<'\t'; 
	} 
	cout<<endl;
	
	for(int i=25;i<33;i++){
		cout<<"R["<<dec<<i<<"]="<<hex<<R[i]<<'\t'; 
	} 
	cout<<endl;
	cout<<endl; 
}

int main(){
	allocMem(4096);
	
	PC=0;
	Program();
	char Do='y';
	while(Do=='y'){

		cout<<"��ִ��ָ��ǰPC=0x"<<PC<<endl; 
		showRegs();
		cout<<endl;
		IR=ReadWord(PC);//��ȡPCָ��Ĵ�����Ӧ�ڴ��ַ�е�ֵ
		nextPC=PC+wordsize; //PC+4 
		
		Decode(IR) ; //���� 
		switch(opcode){
			case LUI:{
				cout<<"ִ��LUIָ��:����������Ϊ��20λ����12λ��0��䣬����Ž�rd�Ĵ���"<<endl;
				R[rd]=imm31_12U_0;
				break;
			}
			case ALUPC:{
				cout<<"ִ��ALUPCָ�����������Ϊ��20λ����12λ��0��䣬������ϴ�ʱPCֵ����rd�Ĵ�����PCֵ������"<<endl;
				R[rd]=imm31_12U_0+PC;
				break;
			}
			case JAL:{
				cout<<"ִ��JALָ����������з�����չ*2+pc��Ϊ�µ�pcֵ������ԭpc+4�Ž�rd�Ĵ���"<<endl;
				R[rd]=PC+4;
				nextPC=PC+imm_sign_31_12J*2;
				break;
			}
			case JALR:{
				cout<<"ִ��JALRָ���ָ���12λ��Ϊ�������з�����չ*2+r1��Ϊ�µ�pcֵ������ԭpc+4�Ž�rd"<<endl;
				R[rd]=PC+4;
				nextPC=R[r1]+imm31_20JR*2;
				break;
			} 
			case BType:{
				switch(func3){
					case BEQ:{
						cout<<"ִ��BEQָ����r1��ֵ=r2��ֵ�����������з�������20λ*2+PC��ΪPCֵ"<<endl;
						if(R[r1]==R[r2]) {
							nextPC=PC+imm_sign_31_25B_11_7B*2;
						}
						break;
					}
					case BNE:{
						cout<<"ִ��BNEָ����r1��ֵ!=r2��ֵ�����������з�������20λ*2+PC��ΪPCֵ"<<endl;
						if(R[r1]!=R[r2]) {
							nextPC=PC+imm_sign_31_25B_11_7B*2;
						}
						break;
					}
					case BLT:{
						cout<<"ִ��BLTָ������з��űȽ�,���r1��ֵ<r2��ֵ�����������з�������20λ*2+PC��ΪPCֵ"<<endl;
						if((int)R[r1]<(int)R[r2]) {//�з��űȽϾ���䵽32λ 
							nextPC=PC+imm_sign_31_25B_11_7B*2;
						}
						break;
					}
					case BGE:{
						cout<<"ִ��BGEָ������з��űȽ�,���r1��ֵ>r2��ֵ�����������з�������20λ*2+PC��ΪPCֵ"<<endl;
						if((int)R[r1]>(int)R[r2]) {
							nextPC=PC+imm_sign_31_25B_11_7B*2;
						}
						break;
					}
					case BLTU:{
						cout<<"ִ��BLTUָ������޷��űȽ�,���r1��ֵ<r2��ֵ�����������з�������20λ*2+PC��ΪPCֵ"<<endl;
						if(R[r1]<R[r2]) {//�з��űȽϾ���䵽32λ 
							nextPC=PC+imm_sign_31_25B_11_7B*2;
						}
						break;
					}
					case BGEU:{
						cout<<"ִ��BGEUָ������޷��űȽ�,���r1��ֵ>r2��ֵ�����������з�������20λ*2+PC��ΪPCֵ"<<endl;
						if(R[r1]>R[r2]) {
							nextPC=PC+imm_sign_31_25B_11_7B*2;
						}
						break;
					}
						default:
						cout << "ERROR: Unknown funct3 in BRANCH instruction " << IR << endl;
				}
				break;
			}
			case LType:{
				switch(func3){
					case LB:{
						cout<<"ִ��LBָ���ָ���12λ��Ϊ�������з�����չ+r1�Ĵ�����ֵ����Ϊ��ַ����ȡ�洢����Ӧ��ַ�е��ֽڲ���չ��32λ����rd�Ĵ���"<<endl;
						R[rd]=(int)ReadByte(R[r1]+imm31_20L,1);//��־λ1������ֵΪ�з��ŵ� 
						break;
					}
					case LH:{
						cout<<"ִ��LHָ���ָ���12λ��Ϊ�������з�����չ+r1�Ĵ�����ֵ����Ϊ��ַ����ȡ�洢����Ӧ��ַ�е�2���ֽڲ���չ��32λ����rd�Ĵ���"<<endl;
						R[rd]=(int)Read2Byte(R[r1]+imm31_20L,1);
						break;
					}
					case LW:{
						cout<<"ִ��LWָ���ָ���12λ��Ϊ�������з�����չ+r1�Ĵ�����ֵ����Ϊ��ַ����ȡ�洢����Ӧ��ַ�е�4���ֽڷ���rd�Ĵ���"<<endl;
						R[rd]=ReadWord(R[r1]+imm31_20L);
						break;
					}
					case LBU:{
						cout<<"ִ��LBUָ���ָ���12λ��Ϊ�������з�����չ+r1�Ĵ�����ֵ����Ϊ��ַ����ȡ�洢����Ӧ��ַ�е��ֽڲ��޷�����չ��32λ����rd�Ĵ���"<<endl;
						R[rd]=(uint32_t)ReadByte(R[r1]+imm31_20L,0);//��־λ1������ֵΪ�з��ŵ� 
						break;
					}
					case LHU:{
						cout<<"ִ��LHָ���ָ���12λ��Ϊ�������з�����չ+r1�Ĵ�����ֵ����Ϊ��ַ����ȡ�洢����Ӧ��ַ�е�2���ֽڲ��޷�����չ��32λ����rd�Ĵ���"<<endl;
						R[rd]=(uint32_t)Read2Byte(R[r1]+imm31_20L,0);
						break;
					}
					default:
						cout<<"��֧�����벻����L��"<<endl;
						break; 
				}
				break;
			}
			case SType:{
				switch(func3){
					case SB:{
						cout<<"ִ��SBָ����������з�����չ32λ��r1�Ĵ�����ӣ���Ϊ�洢����ַ����r2�Ĵ�����ֵ��8λ����洢��"<<endl;
						WriteByte(R[r1]+imm_sign_31_25S_11_7S,(R[r2]&0xff)); 
					//	cout<<imm30_25S<<endl;
					//	cout<<dec<<R[r1]+imm_sign_31_25S_11_7S<<endl;
						cout<<"ִ��ָ�����Ӧ�ڴ�ֵΪ0x"<<ReadByte(R[r1]+imm_sign_31_25S_11_7S,0)<<endl;//���ڲ��� 
						break;
					}
					case SH:{
						cout<<"ִ��SHָ����������з�����չ32λ��r1�Ĵ�����ӣ���Ϊ�洢����ַ����r2�Ĵ�����ֵ��16λ����洢��"<<endl;
						Write2Byte(R[r1]+imm_sign_31_25S_11_7S,(R[r2]&0xffff)); 
						cout<<"ִ��ָ�����Ӧ�ڴ�ֵΪ0x"<<Read2Byte(R[r1]+imm_sign_31_25S_11_7S,0)<<endl;//���ڲ��� 
						break;
					}
					case SW:{
						cout<<"ִ��SWָ����������з�����չ32λ��r1�Ĵ�����ӣ���Ϊ�洢����ַ����r2�Ĵ�����ֵ����洢��"<<endl;
						WriteWord(R[r1]+imm_sign_31_25S_11_7S,R[r2]); 
						cout<<"ִ��ָ�����Ӧ�ڴ�ֵΪ0x"<<ReadWord(R[r1]+imm_sign_31_25S_11_7S)<<endl;//���ڲ��� 
						break;
					}
					default:
						cout<<"��֧�벻����S��"<<endl;
						break;
				}
				break;
			} 
			case IType:{
				switch(func3){
					case ADDI:{
						cout<<"ִ��ADDIָ��:��������������չ��r1��ӣ��������ʡ�Ը�λ��������32λ�Ž�rd��"<<endl;
						R[rd]=(R[r1]+imm_sign_31_20I)&0xffffffff;
						break;
					}
					case SLTI:{
						cout<<"ִ��SLTIָ��:�����з��űȽϣ����r1<imm(�з�����չ)��rd=1"<<endl;
						if((int)R[r1]<imm_sign_31_20I) //ע������Ҫ�Ӹ�int����Ƚϸ���ʱ��ͳ��� 
							R[rd]=1; 
//						cout<<r1<<":"<<(int)R[r1]<<",,,"<<imm_sign_31_20I<<endl; 
						break;
					}
					case SLTIU:{
						cout<<"ִ��SLTIUָ��:�����޷��űȽϣ����r1<imm(�з�����չ)��rd=1"<<endl;
//						cout<<r1<<":"<<(unsigned int)R[r1]<<",,,"<<imm_sign_31_20I<<endl; 
						if((unsigned int)R[r1]<imm_sign_31_20I) 
							R[rd]=1; 
						break;
					}
					case XORI:{
						cout<<"ִ��XORIָ��:������������rd=r1^imm(������չ��32λ)"<<endl;
						R[rd]=R[r1]^imm_sign_31_20I;
						break;
					}
					case ORI:{
						cout<<"ִ��ORIָ��:���л������rd=r1|imm(������չ��32λ)"<<endl;
						R[rd]=R[r1]|imm_sign_31_20I;
						break;
					}
					case ANDI:{
						cout<<"ִ��ANDIָ��:�����������rd=r1&imm(������չ��32λ)"<<endl;
						cout<<r1<<"L"<<R[r1]<<" imm:"<<imm_sign_31_20I<<"int32"<<(int)imm_sign_31_20I<<endl; 
						R[rd]=R[r1]&imm_sign_31_20I;
						break;
					}
					case SLLI:{
						cout<<"ִ��SLLIָ��������Ʋ������������0��rd=r1<<shamt"<<endl;
						R[rd]=R[r1]<<shamt;
						break;
					}
					case SRL_AI:{
						switch(func7){
							case SRLI:{
								cout<<"ִ��SRLTָ������߼����Ʋ�����ǰ�����0��rd=r1>>shamt"<<endl;
								R[rd]=R[r1]>>shamt;
								break;
							}
							case SRAI:{
								cout<<"ִ��SRRTָ������������Ʋ�����ǰ��������λ��rd=r1>>shamt"<<endl;
								R[rd]=(int)R[r1]>>shamt;
								break;
							}
							default:
								cout<<"func7���������"<<endl;
								break; 
						}
						break;
					}
					default:
						cout<<"���ܷ�֧���������I��ָ��"<<endl;
						break; 
				}
				break;
			} 
			case RType:{
				switch(func3){
					case ADD_SUB:{
						switch(func7){
							case ADD:{
								cout<<"ִ��ADDָ�rd=r1+r2��ȡ��32λ���������"<<endl;
								R[rd]=R[r1]+R[r2];
								break;
							}
							case SUB:{
								cout<<"ִ��SUBָ�rd=r1-r2��ȡ��32λ���������"<<endl;
								R[rd]=R[r1]-R[r2];
								break;
							}
							default:
								cout<<"func7����"<<endl;
								break;
						}
						break;
					}
					case SLL:{
						cout<<"ִ��SLLָ�r1�����ƶ� r2ֵ�ĵ�5λ�Σ����������rd��"<<endl;
						R[rd]=R[r1]<<(R[r2]&0x1f);
						break;
					}
					case SLT:{
						cout<<"ִ��SLTָ��з��űȽϣ����r1<r2,��1д��rd��"<<endl;
						if((int)R[r1]<(int)R[r2])
							R[rd]=1;
						break;
					}
					case SLTU:{
						cout<<"ִ��SLTUָ��޷��űȽϣ����r1<r2,��1д��rd��"<<endl;
						if((unsigned int)R[r1]<R[r2])
							R[rd]=1;
						break;
					}
					case XOR:{
						cout<<"ִ��XORָ�rd=r1^r2"<<endl;
						R[rd]=R[r1]^R[r2]; 
						break;
					}
					case SRL_SRA:{
						switch(func7){
							case SRL:{
								cout<<"ִ��SRLָ�r1����r2��5λ�Σ���λ��0"<<endl;
								R[rd]=R[r1]>>(R[r2]&0x1f);
								break;
							}
							case SRA:{
								cout<<"ִ��SRAָ�r1����r2��5λ�Σ���λ������λ"<<endl;
								R[rd]=(int)R[r1]>>(R[r2]&0x1f);
								break;
							}
							default:
								cout<<"SRL_SRA��func7����"<<endl;
								break; 
						}
						break;
					}
					case OR:{
						cout<<"ִ��ORָ�rd=r1|r2"<<endl;
						R[rd]=R[r1]|R[r2]; 
						break;
					}
					case AND:{
						cout<<"ִ��ANDָ�rd=r1&r2"<<endl;
						R[rd]=R[r1]&R[r2]; 
						break;
					}
					default:
						cout<<"����I��ָ��"<<endl;
						break;
				}
				break;
			}
			default:
				cout<<"�����벻����"<<endl;
				break; 
		}
		
		PC=nextPC;
		
		cout<<"ִ��ָ���PC��Ĵ�����ֵ������ʾ��"<<endl;
		showRegs();
		cout<<"��ǰָ��ִ�����"<<endl;
		cout<<"����ִ��ָ�(y/n)"<<endl;
		cin>>Do; 
		cout<<endl;
	}
	return 0;
}





