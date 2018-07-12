#include <cstdint>
//#include <cstdio>
#include <iostream>

using namespace std;

// ��ʾָ��
#define AUIPC 0x17

// ��֧���͵�ָ�����BRANCH���ڱ�ʾ��ͬ��opcode
#define BRANCH 0x63

#define BEQ 0x0
#define BNE 0x1
#define BLT 0x4
#define BGE 0x5
#define BLTU 0x6
#define BGEU 0x7


// װ�����͵�ָ�����LOAD���ڱ�ʾ��ͬ��opcode
#define LOAD 0x03

#define LB 0x0
#define LH 0x1
#define LW 0x2
#define LBU 0x4
#define LHU 0x5


// �������͵�ָ�����STORE���ڱ�ʾ��ͬ��opcode
#define STORE 0x23

#define SB 0x0
#define SH 0x1
#define SW 0x2


// �ѷ���ָ��
#define LUI 0x37
#define JAL 0x6F
#define JALR 0x67

// ��һ��Դ���������������������߼�����ָ����� ALUR1 ��Ϊ��ͬ��opcode����
#define ALUR1 0x13

#define ADDI 0x0
#define SLTI 0x2
#define SLTIU 0x3
#define XORI 0x4
#define ORI 0x6
#define ANDI 0x7
#define SLLI 0x1

#define SHR 0x5

#define SRLI 0x0
#define SRAI 0x20


// Դ�������������ڼĴ����������߼�����ָ����� ALUR2 ��Ϊ��ͬ��opcode����
#define ALUR2 0x33

#define ADDSUB 0x0
#define ADD 0x0
#define SUB 0x20

#define SLL 0x1
#define SLT 0x2
#define SLTU 0x3
#define XOR 0x4
#define OR 0x6
#define AND 0x7

#define SRLA 0x5

#define SRL 0x0
#define SRA 0x20

// ��������ָ��

#define FENCES 0x0F
#define FENCE 0x0
#define FENCE_I 0x1

#define CSRX 0x73

#define CALLBREAK 0x0
#define ECALL 0x0
#define EBREAK 0x1

#define CSRRW 0x1
#define CSRRS 0x2
#define CSRRC 0x3
#define CSRRWI 0x5
#define CSRRSI 0x6
#define CSRRCI 0x7


// �ڴ�ģ����
// �ڴ�ģ�����й�����
const int WORDSIZE = sizeof(uint32_t);
unsigned int MSIZE = 4096;
char* M;

// �ڴ�ģ�����йغ���
int allocMem(uint32_t s) {
		M = new char[s];
		MSIZE = s;

		return s;
}

void freeMem() {
		delete[] M;
		MSIZE = 0;
}

char readByte(unsigned int address) {
	if(address >= MSIZE) {
		cout << "ERROR: address out of range in readByte" << endl;
		return 0;
	}

	return M[address];
}

void writeByte(unsigned int address, char data) {
	if(address >= MSIZE) {
		cout << "ERROR: address out of range in writeByte" << endl;
		return;
	}

	M[address] = data;
}

uint32_t readWord(unsigned int address) {
	if(address >= MSIZE-WORDSIZE) {
		cout << "ERROR: address out of range in readWord" << endl;
		return 0;
	}

	return *((uint32_t*)&(M[address]));
}

void writeWord(unsigned int address, uint32_t data) {
	if(address >= MSIZE-WORDSIZE) {
		cout << "ERROR: address out of range in writeWord" << endl;
		return;
	}

	*((uint32_t*)&(M[address])) = data;
}

// �������ֱ��д��Ҫ���Ե�ָ��
void progMem() {
	// �ӵ�ַ0��ʼд�����ָ��
	writeWord(0, (1 << 12) | (5 << 7) | (AUIPC));
}


// CPUģ�����й�����
uint32_t PC;
uint32_t R[32];
uint32_t IR;

unsigned int opcode;
unsigned int rs1, rs2, rd;
unsigned int funct7, funct3;
// immediate values for I-type, S-type, B-type, U-type, J-type
unsigned int imm11_0i;
unsigned int imm11_5s, imm4_0s;
unsigned int imm12b, imm10_5b, imm4_1b, imm11b;
unsigned int imm31_12u;
unsigned int imm20j, imm10_1j, imm11j, imm19_12j;

// CPUģ�����йغ���
void decode(uint32_t instruction) {
	opcode = instruction & 0x7F;
	rd = (instruction & 0x0F80) >> 7;
	rs1 = (instruction & 0xF8000) >> 15;
	rs2 = (instruction & 0x1F00000) >> 20;
	funct3 = (instruction & 0x7000) >> 12;
	funct7 = instruction >> 25;
	imm11_0i = instruction >> 20;
	imm11_5s = instruction >> 25;
	imm4_0s = (instruction & 0x0F80) >> 7;
	imm12b = instruction >> 31;
	imm10_5b = (instruction >> 25) & 0x3F;
	imm4_1b = (instruction & 0x0F00) >> 8;
	imm11b = (instruction & 0x080) >> 7;
	imm31_12u = instruction >> 12;
	imm20j = instruction >> 31;
	imm10_1j = (instruction >> 21) & 0x3FF;
	imm11j = (instruction >> 20) & 1;
	imm19_12j = (instruction >> 12) & 0x0FF;
}

void showRegs() {
	cout << "PC=" << PC << " " << "IR=" << IR << endl;

	for(int i=0; i<32; i++) {
		cout << "R[" << i << "]=" << R[i] << " ";
	}
	cout << endl;
}

int main(int argc, char const *argv[]) {
	/* code */
	allocMem(4096);
	progMem();

	PC = 0;

	char c = 'Y';

	while(c != 'n') {
		cout << "ִ�е�ǰָ��֮ǰ�Ĵ���������" << endl;
		showRegs();

		IR = readWord(PC);
		PC = PC + WORDSIZE;

		decode(IR);

		switch(opcode) {
			case LUI:
				//TODO: ����ָ��ģ�����:
				break;
			case AUIPC:
				cout << "Do AUIPC" << endl;
				R[rd] = PC + (imm31_12u << 12);
				break;
			case JAL:
				//TODO: ����ָ��ģ�����:
				break;
			case JALR:
				//TODO: ����ָ��ģ�����:
				break;
			case BRANCH:
				switch(funct3) {
					case BEQ:
						//TODO: ����ָ��ģ�����:
						cout << "DO BLTU" << endl;
						unsigned int scr1 =R[rs1];
						unsigned int scr2 = R[rs2];
						unsigned int imm_temp;
						if(scr1==scr2){
							imm_temp=imm12b<<12|imm11b<<11|imm10_5b<<5|imm4_1b<<1;
						}else {
							PC=PC+imm_temp;
						}
						break;
					case BNE:
						//TODO: ����ָ��ģ�����:
						break;
					case BLT:
						//TODO: ����ָ��ģ�����:
						break;
					case BGE:
						//TODO: ����ָ��ģ�����:
						break;
					case BLTU:
						//TODO: ����ָ��ģ�����:
						break;
					case BGEU:
						//TODO: ����ָ��ģ�����:
						break;
					default:
						cout << "ERROR: Unknown funct3 in BRANCH instruction " << IR << endl;
				}
				break;
			case LOAD:
				switch(funct3) {
					case LB:
						//TODO: ����ָ��ģ�����:
						cout << "DO LB" << endl;
						int unsigned data,imm_temp;
						char data;
						imm_temp= imm11_0i>>11; 
						if(imm_temp==1){
							imm_temp = imm11_0i|0xfffff000;
						}else {
							imm_temp= (0|imm11_0i);
						}
						data = writeByte(imm_temp);
						R[rs1]=data;
						break;
					case LH:
						//TODO: ����ָ��ģ�����:
						break;
					case LW:
						//TODO: ����ָ��ģ�����:
						break;
					case LBU:
						//TODO: ����ָ��ģ�����:
						break;
					case LHU:
						//TODO: ����ָ��ģ�����:
						break;
					default:
						cout << "ERROR: Unknown funct3 in LOAD instruction " << IR << endl;
				}
				break;
			case STORE:
				switch(funct3) {
					case SB:
						//TODO: ����ָ��ģ�����:
						break;
					case SH:
						//TODO: ����ָ��ģ�����:
						break;
					case SW:
						//TODO: ����ָ��ģ�����:
						cout << "DO SW" << endl;
						unsigned int imm_temp;
						char d;
						d=R[rs2]&oxffffffff;
						unsigned int a;
						imm_temp=imm11_5s<<5|imm4_0s;
						if(imm11_5s & 0x800) {
							imm_temp = 0xffff0000|imm11_5<<5|imm4_0s;
						}
						a=R[rs1]+imm_temp;
						writeByte(a,d);
						break;
					default:
						cout << "ERROR: Unknown funct3 in STORE instruction " << IR << endl;
				}
				break;
			case ALUR1:
				switch(funct3) {
					case ADDI:
						//TODO: ����ָ��ģ�����:
						break;
					case SLTI:
						//TODO: ����ָ��ģ�����:
						break;
					case SLTIU:
						//TODO: ����ָ��ģ�����:
						break;
					case XORI:
						//TODO: ����ָ��ģ�����:
						break;
					case ORI:
						//TODO: ����ָ��ģ�����:
						break;
					case ANDI:
						//TODO: ����ָ��ģ�����:
						cout << "DO ANDI"<<endl;
						unsigned int re3,imm3;
						imm3=imm11_0i>>11;
						if(imm3==1){
							re3=(0xfffff000|imm11_0i);
						}else{
							re3=(0|imm11_0i);	
						}
						R[rd]=R[rs1]&re3;
						break;
					case SLLI:
						//TODO: ����ָ��ģ�����:
						break;
					case SHR:
						switch(imm11_0i >> 5) {
							case SRLI:
								//TODO: ����ָ��ģ�����:
								break;
							case SRAI:
								//TODO: ����ָ��ģ�����:
								break;
							default:
								cout << "ERROR: unknown (imm11_0i >> 5) in ALUR1 SHR instruction " << IR << endl;
						}
						break;
					default:
						cout << "ERROR: unknown funct3 in ALUR1 instruction " << IR << endl;
				}
				break;
			case ALUR2:
				switch(funct3) {
					case ADDSUB:
						switch(funct7) {
							case ADD:
								//TODO: ����ָ��ģ�����:
								break;
							case SUB:
								//TODO: ����ָ��ģ�����:
								break;
							default:
								cout << "ERROR: unknown funct7 in ALUR2 ADDSUB instruction " << IR << endl;
						}
						break;
					case SLL:
						//TODO: ����ָ��ģ�����:
						cout << "DO SLL" << endl;
						unsigned int rsTransform;
						rsTransform = R[rs1]&0x1f;
						R[rs2] << rsTransform;
						break;
					case SLT:
						//TODO: ����ָ��ģ�����:
						break;
					case SLTU:
						//TODO: ����ָ��ģ�����:
						break;
					case XOR:
						//TODO: ����ָ��ģ�����:
						break;
					case OR:
						//TODO: ����ָ��ģ�����:
						break;
					case AND:
						//TODO: ����ָ��ģ�����:
						break;
					case SRLA:
						switch(funct7) {
							case SRL:
								//TODO: ����ָ��ģ�����:
								break;
							case SRA:
								//TODO: ����ָ��ģ�����:
								break;
							default:
								cout << "ERROR: unknown funct7 in ALUR2 SRLA instruction " << IR << endl;
						}
						break;
					default:
						cout << "ERROR: unknown funct3 in ALUR2 instruction " << IR << endl;
				}
				break;
			case FENCES:
				switch(funct3) {
					case FENCE:
						//TODO: ����ָ��ģ�����:
						break;
					case FENCE_I:
						//TODO: ����ָ��ģ�����:
						break;
					default:
						cout << "ERROR: unknown funct3 in FENCES instruction " << IR << endl;
				}
				break;
			case CSRX:
				switch(funct3) {
					case CALLBREAK:
						switch(imm11_0i) {
							case ECALL:
								//TODO: ����ָ��ģ�����:
								break;
							case EBREAK:
								//TODO: ����ָ��ģ�����:
								break;
							default:
								cout << "ERROR: unknown imm11_0i in CSRX CALLBREAK instruction " << IR << endl;
						}
						break;
					case CSRRW:
						//TODO: ����ָ��ģ�����:
						break;
					case CSRRS:
						//TODO: ����ָ��ģ�����:
						break;
					case CSRRC:
						//TODO: ����ָ��ģ�����:
						break;
					case CSRRWI:
						//TODO: ����ָ��ģ�����:
						break;
					case CSRRSI:
						//TODO: ����ָ��ģ�����:
						break;
					case CSRRCI:
						//TODO: ����ָ��ģ�����:
						break;
					default:
						cout << "ERROR: unknown funct3 in CSRX instruction " << IR << endl;
				}
				break;
			default:
				cout << "ERROR: Unkown instruction " << IR << endl;
				break;
		}

		cout << "ִ�е�ǰָ��֮��Ĵ���������" << endl;
		showRegs();

		cout << "����ģ�⣿��Y/n)" << endl;
		cin.get(c);
	}

	freeMem();

	return 0;
}


