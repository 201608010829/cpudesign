#include <cstdint>
#include <iostream>

using namespace std;

// Instructions identified by opcode  �������ʶ��ָ��
#define AUIPC 0x17
#define LUI 0x37
#define JAL 0x6F
#define JALR 0x67


// Branches using BRANCH as the label for common opcode ʹ�÷�֧��Ϊ���ò������ǩ�ķ�֧
#define BRANCH 0x63

#define BEQ 0x0
#define BNE 0x1
#define BLT 0x4
#define BGE 0x5
#define BLTU 0x6
#define BGEU 0x7


// Loads using LOAD as the label for common opcode ʹ��LOAD��Ϊ���ò������ǩ����
#define LOAD 0x03

#define LB 0x0
#define LH 0x1
#define LW 0x2
#define LBU 0x4
#define LHU 0x5


// Stores using STORE as the label for common opcode ʹ��store��Ϊ���ò������ǩ�Ĵ洢
#define STORE 0x23

#define SB 0x0
#define SH 0x1
#define SW 0x2


// ALU ops with one immediate ��һ����ʱ��ALU����
#define ALUIMM 0x13

#define ADDI 0x0
#define SLTI 0x2
#define SLTIU 0x3
#define XORI 0x4
#define ORI 0x6
#define ANDI 0x7
#define SLLI 0x1

#define SHR 0x5  // common funct3 for SRLI and SRAI srli��srai�Ĺ�������3

#define SRLI 0x0
#define SRAI 0x20


// ALU ops with all register operands  ���мĴ���������������
#define ALURRR 0x33

#define ADDSUB 0x0  // common funct3 for ADD and SUB  ADD��SUB�Ĺ��ú��� 
#define ADD 0x0
#define SUB 0x20

#define SLL 0x1
#define SLT 0x2
#define SLTU 0x3
#define XOR 0x4
#define OR 0x6
#define AND 0x7

#define SRLA 0x5  // common funct3 for SRL and SRA     SRL��SRA�Ĺ��ú���3 

#define SRL 0x0
#define SRA 0x20

// Fences using FENCES as the label for common opcode   ʹ��Χ����Ϊ���ò������ǩ��Χ��

#define FENCES 0x0F
#define FENCE 0x0
#define FENCE_I 0x1

// CSR related instructions   CSR���˵�� 
#define CSRX 0x73

#define CALLBREAK 0x0  // common funct3 for ECALL and EBREAK
#define ECALL 0x0
#define EBREAK 0x1

#define CSRRW 0x1
#define CSRRS 0x2
#define CSRRC 0x3
#define CSRRWI 0x5
#define CSRRSI 0x6
#define CSRRCI 0x7


// Data for memory
const int WORDSIZE = sizeof(uint32_t);
unsigned int MSize = 4096;//MSize�Ǵ洢�ռ��С
char* M;//������һ���ַ��͵�ָ�룬���������ָ���洢�ռ�� 
unsigned int ebreakadd = 4; 
// Functions for memory
int allocMem(uint32_t s) {//�ڴ溯��  �����ڴ溯��������Ϊ�����ڴ�Ĵ�С��ͨ��Mָ����� 
	M = new char[s];//������һ��s��С�Ķ�̬���� 
	MSize = s;//��s��ֵ��MSize 
    for(int i=0;i<s;i++){
    	//cout<<"zero";
    	M[i]=0;
	}
	return s;
}

void freeMem() {//ɾ��Mָ������� 
	delete[] M;
	MSize = 0;
}

char readByte(unsigned int address) {//����ĳһ����ַ�ϵ�Byte ������һ����ַ  ͨ������ֵ���� 
	if(address >= MSize) {//������Χ�򱨴� 
		cout << "ERROR: Address out of range in readByte" << endl;
		return 0;
	}
	return M[address];//ע��M��char���͵�ָ�룬���������Ƿ���һ��char���͵����� 
}

void writeByte(unsigned int address, char data) {//дһbyte����  ������һ����ַ�Լ�Ҫд������� 
	if(address >= MSize) {
		cout << "ERROR: Address out of range in writeByte" << endl;
		return;
	}
	M[address] = data;//����Ҳ��д��һ��char���͵����ݣ�1byte����8bit����һ��char 
}

uint32_t readWord(unsigned int address) {                                //��ʵ�Ͷ�byte�������ƣ������Ѷ���������ת������32λ���ͣ����ԣ���һ�ζ�1��Word��4Byte���ݣ�������1byte���� 
	if(address >= MSize-WORDSIZE) {
		cout << "ERROR: Address out of range in readWord" << endl;
		return 0;
	}
	return *((uint32_t*)&(M[address]));
}

uint32_t readHalfWord(unsigned int address){//һ���Զ�2byte���ݼ�һ���Word���� 
	if(address >= MSize-WORDSIZE/2) {
		cout << "ERROR: Address out of range in readWord" << endl;
		return 0;
	}
	return *((uint16_t*)&(M[address]));
}

void writeWord(unsigned int address, uint32_t data) {//д4��byte���� 
	if(address >= MSize-WORDSIZE) {
		cout << "ERROR: Address out of range in writeWord" << endl;
		return;
	}
//IR��ָ��Ĵ��� 
	*((uint32_t*)&(M[address])) = data;
}

void writeHalfWord(unsigned int address, uint32_t data) {//д���word������ 
	if(address >= MSize-WORDSIZE/2) {
		cout << "ERROR: Address out of range in writeWord" << endl;
		return;
	}
	*((uint16_t*)&(M[address])) = data;
}

// Write memory with instructions to test
void m_progMem(){
	writeWord(0, (0x666 << 12) | (2 << 7) | (LUI));//ָ����ڵ�2���Ĵ���д��0x666
	writeWord(4, (1 << 12) | (3 << 7) | (AUIPC));//ָ����ڵ�3���Ĵ�����д��PC+0x1000
	writeWord(8, (0x66 << 12) | (5 << 7) | (LUI));//ָ����ڵ�5���Ĵ���д��6
	writeWord(12, (0x0<<25) | (5<<20) | (0<<15) | (SW << 12) | (0x1a << 7) | (STORE));//��(0�żĴ�����ֵ����0x1a)��ַд��5�żĴ����е�ֵ 
	writeWord(16, (0x10<<20) | (0<<15) | (LBU<<12) | (4<<7) | (LOAD));//��ȡ0x10��ַ�ϵ�1byteȡ���8λд��4�żĴ��� 
	writeWord(20, (0x0<<25) | (2<<20) | (0<<15) | (BGE<<12) | (0x8<<7) | (BRANCH));//�ж�0�żĴ�����2�żĴ���ֵ�Ĵ�С��������ڵ������޸�NextPCΪ PC + Imm12_1BtypeSignExtended;
} 


// ============================================================================


//data for CPU   CPU�е����� 
uint32_t PC, NextPC;
uint32_t R[32];
uint32_t IR;  //����ʵ��pc����ָ��Ĵ���������������ʵ��ָ�� 

unsigned int opcode;   //������ 
unsigned int rs1, rs2, rd;   //�����Ĵ���? 
unsigned int funct7, funct3;   //�������funct7����funct3 
unsigned int shamt;            
unsigned int pred, succ;
unsigned int csr, zimm;

// immediate values for I-type, S-type, B-type, U-type, J-type  ���ֲ�ͬ���͵������������ԴӺ�׺���� 
unsigned int imm11_0i;
unsigned int imm11_5s, imm4_0s;
unsigned int imm12b, imm10_5b, imm4_1b, imm11b;
unsigned int imm31_12u;
unsigned int imm20j, imm10_1j, imm11j, imm19_12j;

unsigned int imm_temp;
unsigned int src1,src2;


unsigned int Imm11_0ItypeZeroExtended;
int Imm11_0ItypeSignExtended;
int Imm11_0StypeSignExtended;
unsigned int Imm12_1BtypeZeroExtended;
int Imm12_1BtypeSignExtended;
unsigned int Imm31_12UtypeZeroFilled;
int Imm20_1JtypeSignExtended;
int Imm20_1JtypeZeroExtended;

// Functions for CPU
void decode(uint32_t instruction) {//decode���������˼��RV32Iָ��4���ֽ� 
	// Extract all bit fields from instruction  ��ָ������ȡ����λ�ֶ� 
	opcode = instruction & 0x7F;//��ȡ��7λ����0~6λ 
	rd = (instruction & 0x0F80) >> 7;//��ȡ�ӵ����ߵ�7~11λ
	rs1 = (instruction & 0xF8000) >> 15;//��ȡ��15~19λ���õ���һ���Ĵ��� 
	zimm = rs1;//zimm�����Ƕ����һ��unsigned int����rs1��ֵ������ 
	rs2 = (instruction & 0x1F00000) >> 20;//��ȡ��20~24λ���õ��ڶ����Ĵ��� 
	shamt = rs2;//shamt�����Ƕ����һ��unsigned int�� ��rs2��ֵ������ 
	funct3 = (instruction & 0x7000) >> 12;//��ȡ��12~14λ 
	funct7 = instruction >> 25;//��ȡ25~31λ? 
	imm11_0i = ((int32_t)instruction) >> 20;//ת�����з��ŵ����ƶ�����Ӧ��Itype���͵ĵ�ַ 
	csr = instruction >> 20;//��ȡ20~31λ��Ӧ���������imm11_0i��࣬�������޷������͵� 
	imm11_5s = ((int32_t)instruction) >> 25;//��ȡ��25~31λ���ݣ���Ӧ��Stype���͵ĵ�ַ 
	imm4_0s = (instruction >> 7) & 0x01F;//��ȡ��7~11λ���ݣ���ӦStype���͵ĵ�ַ 
	imm12b = ((int32_t)instruction) >> 31;//��ȡ��31λ���ݣ���ӦBtype���͵ĵ�ַ 
	imm10_5b = (instruction >> 25) & 0x3F;//��ȡ��25~30λ���ݣ���ӦBtype���͵ĵ�ַ 
	imm4_1b = (instruction & 0x0F00) >> 8;//��8~11λ����ӦBtype���͵ĵ�ַ 
	imm11b = (instruction & 0x080) >> 7;//��7λ����ӦBtype���͵ĵ�ַ 
	imm31_12u = instruction >> 12;//��12~31λ����ӦUtype���͵ĵ�ַ 
	imm20j = ((int32_t)instruction) >> 31;//��31λ����Ӧjtype���͵ĵ�ַ 
	imm10_1j = (instruction >> 21) & 0x3FF;//��21~31λ����Ӧjtype���͵ĵ�ַ 
	imm11j = (instruction >> 20) & 1;//��20λ����Ӧjtype���͵ĵ�ַ 
	imm19_12j = (instruction >> 12) & 0x0FF;//��12��19λ����Ӧjtype���͵ĵ�ַ 
	pred = (instruction >> 24) & 0x0F;
	succ = (instruction >> 20) & 0x0F;

	// ========================================================================
	// Get values of rs1 and rs2  ��ȡ�Ĵ�����ֵ 
	src1 = R[rs1];
	src2 = R[rs2];

	// Immediate values   ������ֵ 
	Imm11_0ItypeZeroExtended = imm11_0i & 0x0FFF;//��Ϊ�ܹ���12λ��ȥ������λ��չ�������Itype�������� 
	Imm11_0ItypeSignExtended = imm11_0i;

	Imm11_0StypeSignExtended = (imm11_5s << 5) | imm4_0s;//�����Stype�������� 

	Imm12_1BtypeZeroExtended = imm12b & 0x00001000 | (imm11b << 11) | (imm10_5b << 5) | (imm4_1b << 1);//�����Btype�������� 
	Imm12_1BtypeSignExtended = imm12b & 0xFFFFF000 | (imm11b << 11) | (imm10_5b << 5) | (imm4_1b << 1);

	Imm31_12UtypeZeroFilled = instruction & 0xFFFFF000;//�����Utype�������� 

	Imm20_1JtypeSignExtended = (imm20j & 0xFFF00000) | (imm19_12j << 12) | (imm11j << 11) | (imm10_1j << 1);//�����Jtype��������  
	Imm20_1JtypeZeroExtended = (imm20j & 0x00100000) | (imm19_12j << 12) | (imm11j << 11) | (imm10_1j << 1);
	// ========================================================================
}

void show32Mess(){
	cout << endl << endl;
	for(int i=0; i<32; i++) {                                                                          //��ӡ32���Ĵ�����ֵ 
	    char tp = M[i];                                                                                //cout<<(int)tp<<endl;
		cout << "M[" << i << "]=0x"  << ((unsigned int)tp&0x000000ff)<<" ";
	}
	cout << endl << endl;
}

void showRegs() {                                                                                      //������ӡ�Ĵ�����ֵ���ܹ���32���Ĵ���,�Ĵ���������R������ 
                                                                                                       //��ӡpc��ֵ 
	cout << "PC=0x" << std::hex << PC << " " << "IR=0x" << std::hex << IR << endl;
    show32Mess();
	for(int i=0; i<32; i++) {                                                                          //��ӡ32���Ĵ�����ֵ 
		cout << "R[" << i << "]=0x" << std::hex << R[i] << " ";
	}
	cout << endl<<endl;
}




int main(int argc, char const *argv[]) {
	/* code */
	allocMem(4096);//����4096byte��С�Ŀռ� ,��Mָ��һ��4096byte��С�Ŀռ䣬Ȼ��s��ֵΪ4096 
	m_progMem();//Ӧ���Ƕ��ڴ����һЩ��ʼ���Ĳ����ɣ�ִ����һ�Ѷ�����ռ����д��Ĳ�������ϸ�Ŀ��Կ�����ʵ�� 

	PC = 0;//pc��ʼΪ0 

	char c = 'Y';//��ʼ��cΪY��Ӧ���������жϳ�������İ� 

	while(c != 'n') {
		cout << "Registers bofore executing the instruction @0x" << std::hex << PC << endl;
		showRegs();                                                                              //ÿ��ѭ����ʾһ�¼Ĵ��� 
		IR = readWord(PC);                                                                       //��ȡpc��Ӧ��ָ�һ��ָ����һ��Word����4byte 
		NextPC = PC + WORDSIZE;                                                                  //��ֵ��һ��PC 
		decode(IR);                                                                              //����ָ�� 
		switch(opcode) {                                                                         //�������decodeʱ�ĵ�7λ��ֵ���ǲ����� 
			case LUI:                                                                            // ִ�еĲ���load upper imm����ʵӦ���Ǽ���ָ��� 
				cout << "Do LUI" << endl;
				R[rd] = Imm31_12UtypeZeroFilled;                                                 //����rd��decodeȡ������ֵ����IR�и�20λ��ֵ������ʹ�õ�Utypeָ�ȡ���� 
				break;
			case AUIPC:                                                                          //0x17���ڽ���PC��Ե�ַ��ʹ��U�͸�ʽ����0�����͵�12λ�� ����ƫ������ӵ�AUIPCָ��ĵ�ַ��Ȼ�󽫽������Ĵ���
				cout << "Do AUIPC" << endl;
				cout << "PC = " << PC << endl;
				cout << "Imm31_12UtypeZeroFilled = " << Imm31_12UtypeZeroFilled << endl;
				R[rd] = PC + Imm31_12UtypeZeroFilled;
				break;
			case JAL:                                                                              //0x6F,��������ת 
				cout << "Do JAL" << endl;
				R[rd]=PC+4;
				NextPC = PC+ Imm20_1JtypeSignExtended;    
				break;
			case JALR:                                                                             //0x67,��������ת��ֱ����תָ���������ת���ɼĴ���rs1ָ����ָ�������һ��ָ��ĵ�ַ���浽�Ĵ���rd�� 
				cout << "DO JALR" << endl;
				R[rd]=PC+4;
				NextPC=R[rs1]+Imm20_1JtypeSignExtended;
				break;
			case BRANCH://0x63��ָ֧�� ���е�BRANCHָ��õ���B���͸�ʽ������ָ�����������Ǵ���ƫ���� 
				switch(funct3) {
					case BEQ://0x0��src1��src2�Ĵ�����ȵ�ʱ��ִ�� 
						cout << "DO BEQ" << endl;
						if(src1==src2){
							NextPC = PC + Imm12_1BtypeSignExtended;
						}
						break;
					case BNE://0x1��src1��src2�Ĵ�������ȵ�ʱ��ִ�� 
						cout << "Do BNE " << endl;
						if(src1!=src2){
							NextPC = PC + Imm12_1BtypeSignExtended;
						}
						break;
					case BLT://0x4�з��űȽϵ�src1<src2ʱִ�� 
						cout << "Do BLT" << endl;
						if((int)src1<(int)src2){
							NextPC = PC + Imm12_1BtypeSignExtended;
						}
						break;
					case BGE://0x5�з��űȽϵ�src1>=src2ʱִ�� 
						cout << "Do BGE" << endl;
						cout<<"src1Ϊ "<<src1<<endl;
						cout<<"src2Ϊ "<<src2<<endl;
						cout<<"immΪ "<<Imm12_1BtypeSignExtended<<endl; 
						if((int)src1 >= (int)src2)
							NextPC = PC + Imm12_1BtypeSignExtended;
						break;
					case BLTU://0x6
						cout << "Do BLTU" << endl;
						if(src1<src2){
							NextPC=PC+Imm12_1BtypeSignExtended;
						}
						break;
					case BGEU://0x7
						cout<<"Do BGEU"<<endl;

						if(src1>=src2){
							NextPC=PC+Imm12_1BtypeSignExtended;
						}    
						break;
					default://�Ҳ�����Ӧ��ָ�� 
						cout << "ERROR: Unknown funct3 in BRANCH instruction " << IR << endl;
				}
				break;
			case LOAD://0x03 LOAD������ΪI����  Loads copy a value from memory to register rd
			/*The LW instruction loads a 32-bit value from memory into rd. LH loads a 16-bit value from memory,
then sign-extends to 32-bits before storing in rd. LHU loads a 16-bit value from memory but then
zero extends to 32-bits before storing in rd. LB and LBU are defined analogously for 8-bit values.*/ 
				switch(funct3) {
					case LB://����һ��byte 
						cout << "DO LB" << endl;
						unsigned int LB_LH,LB_LH_UP;
						cout << "LB Address is: " << src1+Imm11_0ItypeSignExtended << endl;
						LB_LH=readByte(src1+Imm11_0ItypeSignExtended);
						LB_LH_UP=LB_LH>>7;
						if(LB_LH_UP==1){//����λ��չ 
							//LB_LH=0xffffff00 & LB_LH;
							LB_LH=0xffffff00 | LB_LH;							
						}else{
							LB_LH=0x000000ff & LB_LH;
						}
						R[rd]=LB_LH; 
						break;
					case LH://
						cout << "Do LH" << endl;
						unsigned int temp_LH,temp_LH_UP;
						temp_LH=readHalfWord(src1+Imm11_0ItypeSignExtended);//Itypeֻ��һ��Դsrc1 
						temp_LH_UP=temp_LH>>15;
						if(temp_LH_UP==1){//ִ�з���λ��չ 
							temp_LH=0xffff0000 | temp_LH;
						}else{
							temp_LH=0x0000ffff & temp_LH;
						}
						R[rd]=temp_LH; 
						break;
					case LW:
						cout << "Do LW" << endl;
						unsigned int temp_LW,temp_LW_UP;
						temp_LW=readByte(src1+Imm11_0ItypeSignExtended);//����ΪʲôҪ��readByte 
						temp_LW_UP=temp_LW>>31;
						if(temp_LW_UP==1){
							temp_LW=0x00000000 | temp_LW;
						}else{
							temp_LW=0xffffffff & temp_LW;
						}
						R[rd]=temp_LW;
						break;
					case LBU:
						cout << "Do LBU" << endl;
						R[rd] = readByte(Imm11_0ItypeSignExtended + src1) & 0x000000ff;
						break;
					case LHU:
						cout << "Do LHU" << endl;
						R[rd] = readByte(Imm11_0ItypeSignExtended + src1) & 0x0000ffff;
						break;
					default://û���ҵ�ָ�� 
						cout << "ERROR: Unknown funct3 in LOAD instruction " << IR << endl;
				}
				break;
			case STORE://STOREָ�� STORE������ΪS����  Stores copy the value in register rs2 to memory. Stype��sr1��sr2 
			/*
			The SW, SH, and SB instructions store 32-bit, 16-bit, and 8-bit values from the low bits of registerrs2 to memory.
			*/ 
				switch(funct3) {//sr1ָ���˵�ַ��sr2ָ���˱����ֵ 
					case SB:
						cout << "Do SB" << endl;
						char sb_d1;
						unsigned int sb_a1;
						sb_d1=R[rs2] & 0xff;//���ֻ��д8λ 
						sb_a1 = R[rs1] +Imm11_0StypeSignExtended;
						writeByte(sb_a1, sb_d1);
						break;
					case SH:
						cout<<"Do SH"<<endl;
						uint16_t j;
						j=R[rs2]&0xffff;//���ֻ��д16λ 
						unsigned int x;
						x = R[rs1] + Imm11_0StypeSignExtended;
						writeHalfWord(x,j);
						break;
					case SW:
						cout << "DO SW" << endl;
						//unsigned int imm_temp;
						uint32_t _swData;
						_swData=R[rs2] & 0xffffffff;
						unsigned int _swR;
						_swR = R[rs1] + Imm11_0StypeSignExtended;
						cout << "SW Addr and Data are: " << _swR << ", " << _swData << endl;
						writeWord(_swR, _swData);
						break;
					default:
						cout << "ERROR: Unknown funct3 in STORE instruction " << IR << endl;
				}
				break;
			case ALUIMM://ALUIMMָ�� 
				switch(funct3) {
					case ADDI:
						cout <<    "Do ADDI" << endl;
						R[rd]=src1+Imm11_0ItypeSignExtended;
						break;
					case SLTI:
						cout << "Do SLTI" << endl;
						if(src1<Imm11_0ItypeSignExtended)
							R[rd] = 1;
						else
							R[rd] = 0;
						break;
					case SLTIU:
						cout << "Do SLTIU" << endl;
						if(src1<(unsigned int)Imm11_0ItypeSignExtended)
							R[rd] = 1;
						else
							R[rd] = 0;
						break;
					case XORI:
						cout << "Do XORI" << endl;
						R[rd]=(Imm11_0ItypeSignExtended)^R[rs1];
						break;
					case ORI:
						cout<<"Do ORI"<<endl;
						R[rd]=R[rs1]|Imm11_0ItypeSignExtended;
						break;
					case ANDI:
						cout << "DO ANDI"<<endl;
						R[rd]=R[rs1]&Imm11_0ItypeSignExtended;
						break;
					case SLLI:
						cout << "Do SLLI " << endl;
						R[rd]=src1<<shamt;
						break;
					case SHR:
						switch(funct7) {
							case SRLI:
								cout << "Do SRLI" << endl;
								R[rd]=src1>>shamt;//�����shamt�Ǵ�sr2ȡ�������� 
								break;
							case SRAI:
								cout << "Do SRAI" << endl;
								R[rd] = ((int)src1) >> shamt;
								break;
							default:
								cout << "ERROR: Unknown (imm11_0i >> 5) in ALUIMM SHR instruction " << IR << endl;
						}
						break;
					default:
						cout << "ERROR: Unknown funct3 in ALUIMM instruction " << IR << endl;
				}
				break;
			case ALURRR://ALURRRָ�� 
				switch(funct3) {
					case ADDSUB:
						switch(funct7) {
							case ADD:
								cout << "Do ADD" << endl;
								R[rd]=R[rs1]+R[rs2];
								break;
							case SUB:
								cout<<" Do SUB"<<endl;
								R[rd]=R[rs1]-R[rs2];
								break;
							default:
								cout << "ERROR: Unknown funct7 in ALURRR ADDSUB instruction " << IR << endl;
						}
						break;
					case SLL:
						cout<<"DO SLL"<<endl;
						unsigned int rsTransform;
						rsTransform=R[rs2]&0x1f;//�������32λ 
						R[rd]=R[rs1]<<rsTransform; 
						break;
					case SLT:
						cout << "Do SLT " << endl;
						if((int)src1<(int)src2){
							R[rd]=1;
						}else{
							R[rd]=0;
						}
						break;
					case SLTU:
						cout << "Do SLTU" << endl;
						if(src2!=0){
							R[rd]=1;
						}else{
							R[rd]=0;
						}
						break;
					case XOR:
						cout << "Do XOR " << endl;
						R[rd]=R[rs1]^R[rs2];
						break;
					case OR:
						cout << "Do OR" << endl;
								R[rd]=R[rs1]|R[rs2];
							break;
					case AND://��ָ�� 
						cout << "Do AND" << endl;
								R[rd]=R[rs1]&R[rs2];
						break;

					case SRLA://����ָ�� 
						switch(funct7) {
							case SRL:
				           cout<<"DO SRL"<<endl;
                                           R[rd]=R[rs1]>>R[rs2];
								break;
							case SRA:
								  cout<<"DO SRA"<<endl;
								  R[rd]=(int)src1>>src2;
								break;
							default:
								cout << "ERROR: Unknown funct7 in ALURRR SRLA instruction " << IR << endl;
						}
						break;
					default:
						cout << "ERROR: Unknown funct3 in ALURRR instruction " << IR << endl;
				}
				break;
			case FENCES://FENCESָ�� 
				switch(funct3) {
					case FENCE:
						//TODO: Fill code for the instruction here
						break;
					case FENCE_I:
						//TODO: Fill code for the instruction here
						cout<<"this is test IR "<<IR<<endl;
						cout<<"fence_i,nop"<<endl;
						break;
					default:
						cout << "ERROR: Unknown funct3 in FENCES instruction " << IR << endl;
				}
				break;
			case CSRX://CSRXָ��  Itype
			//cout << "this is EBREAK !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl; 						
				switch(funct3) {
					case CALLBREAK:
						switch(Imm11_0ItypeZeroExtended) {
							case ECALL:
								//TODO: Fill code for the instruction here
								break;
							case EBREAK:
								{//TODO: Fill code for the instruction here
								    //cout << "this is EBREAK !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl; 
									NextPC = ebreakadd;
									cout << "do ebreak and pc jumps to :" << ebreakadd << endl;
									break;
								}
							default:
								cout << "ERROR: Unknown imm11_0i in CSRX CALLBREAK instruction " << IR << endl;
						}
						break;
					case CSRRW://The CSRRW (Atomic Read/Write CSR) instruction atomically swaps values in the CSRs and integer registers
						/*CSRRWָ���ȡ�ɵ�CSR��ֵ������0��չ��д�������Ĵ���rd��rs1�ĳ�ʼֵд��CSR�У����rdΪ0����˵�����ܶ�CSR���κβ���*/
						//TODO: Fill code for the instruction here
						break;
					case CSRRS:
						/*CSRRS��ȡCSR�е�ֵ��0��չ��Ȼ����д�뵽���ͼĴ���rd��rs1�ĳ�ʼֵ������һ��λ����ָ��Ҫ��CSR��Ҫ���õ�λλ�ã����csrλ��д��rs1�е��κθ�λ����������csr��������Ӧ��λ��csr�е�����λ����Ӱ�죨����csr��д��ʱ���ܻ���������ã���*/ 
						//TODO: Fill code for the instruction here
						{
						    uint32_t temp = readWord(rs2)&0x00000fff;
							uint32_t temp1 = rs1 & 0x000fffff;
//							cout<<"tempֵΪ0x"<<temp<<endl;
//							cout<<"temp1ֵΪ0x"<<temp1<<endl;
//							cout<<"rd��ֵΪ0x"<<rd<<endl;
//							cout<<"д��rd��ֵΪ0x"<<(temp|temp1)<<endl; 
							writeWord(rd,(temp|temp1));
							cout << "do CSRRS and the result is :" << "rd="<<readWord(rd)<<endl;
							break;
						}
					case CSRRC://����CSRRS��CSRRC�����rs1==x0����ָ���дCSR�Ĵ��� 
						/*��ȡCSR��ֵ��0��չ��д��rd�Ĵ����������Ĵ���rs1�еĳ�ʼֵ����Ϊָ��Ҫ��csr�������λλ�õ�λ���롣���CSRλ�ǿ�д�ģ���ôRS1�е��κθ�λ���ᵼ����Ӧ��λ��CSR�б������CSR�е�����λ����Ӱ�졣*/ 
						//TODO: Fill code for the instruction here
						break;
					case CSRRWI:
						//TODO: Fill code for the instruction here
						{	
						    if (rd == 0) break;
							else
							{
								uint32_t zmm = imm11j& 0x000001f;
								uint32_t tem = readWord(rs2) & 0x00000fff;
//								cout<<"rd��ֵΪ0x"<<rd<<endl;
//								cout<<"rs2��ֵΪ0x"<<rs2<<endl;
//								cout<<"zmm��ֵΪ0x"<<zmm<<endl;
//								cout<<"tem��ֵΪ0x"<<tem<<endl; 
								writeWord(rd, tem);
								writeWord(rs2, zmm);
								cout << "do CSRRWI and the result is :" << "rd=" << readWord(rd) << endl;
								break;
							}
						}
					case CSRRSI:
						//TODO: Fill code for the instruction here
						break;
					case CSRRCI:
						//TODO: Fill code for the instruction here
						{	
						    uint32_t zmm = imm11j & 0x000001f;
							uint32_t tem = readWord(rs2) & 0x00000fff;
							if (readWord(rd) != 0)
							{
//								cout<<"rd��ֵΪ0x"<<rd<<endl;
//								cout<<"rs2��ֵΪ0x"<<rs2<<endl;
//								cout<<"zmm��ֵΪ0x"<<zmm<<endl;
//								cout<<"tem��ֵΪ0x"<<tem<<endl; 
								writeWord(rs2, zmm | tem);
							}
							cout << "do CSRRCI and the result is :" << "rd=" << readWord(rd) << endl;
							break;
						}
					default:
						cout << "ERROR: Unknown funct3 in CSRX instruction " << IR << endl;
				}
				break;
			default:
				cout << "ERROR: Unkown instruction " << IR << endl;
				break;
		}

		//Update PC
		PC = NextPC;

		cout << "Registers after executing the instruction" << endl;
		showRegs();
		cout << "Continue simulation (Y/n)? [Y]" << endl;
		cin.get(c);	
		getchar();
	}

	freeMem();

	return 0;
}



