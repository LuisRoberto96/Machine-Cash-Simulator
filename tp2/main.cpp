#include "geral.hpp"
#define TC1 16
#define TC2 16
#define TC3 64
#define TAM_RAM 5000
#define TAM_INSTRUCAO 500
	typedef struct tipo_endereco {
	int endBloco;
	int endPalavra;
}TEndereco;

typedef struct instrucao {
	int opcode;
	TEndereco endereco1;
	TEndereco endereco2;
}Instrucao;

typedef struct linha {
	int cachehit;
	int palavras[4];
	int endBloco;
	int atualiza;
}Linha;

void insere_enderecos(Instrucao[]);
void deleta_ram(int**,int);
void cria_ram(int**,int,int);
void zera_cache(Linha *, int);
void atribui_cache(int,Linha*,int,TEndereco);
int busca_cache(TEndereco,Linha*, int,int,int &);
void atualiza_ram(int **,Linha*,int);
void Troca_Caches(Linha*,int,Linha*);
void pega_da_Ram(int,int**,Linha*,int);
void pega_da_Cache(int,Linha *,int,Linha *,int);
void Maquina(Instrucao[],int,int&,int&,int&,int&,int&,int&,int,int,int,Linha*,Linha*,Linha*,int**);
void imprime_ram(int**,int,int);
void multiplica(Instrucao[],int,int&,int&,int&,int&,int&,int&,int,int,int,Linha*,Linha*,Linha*,int**);
int MMU(TEndereco,int&,int&,int&,int&,int&,int&,int,int,int,Linha*,Linha*,Linha*,int**);

int main(){
	Instrucao operacao[TAM_INSTRUCAO];
	int pc=0,hit1=0,hit2=0,hit3=0,miss1=0,miss2=0,miss3=0;
	int val1,val2;
	double custo=0.0;
	long int custo2=0;
	//insere_enderecos(operacao);
	Linha *cache1=new Linha[TC1], *cache2=new Linha[TC2], *cache3=new Linha[TC3];
	zera_cache(cache1, TC1);
	zera_cache(cache2, TC2);
	zera_cache(cache3, TC2);
	int **ram= new int*[TAM_RAM];
	cria_ram(ram,TAM_RAM,4);
	ifstream ler;
		ler.open("instrucoes.txt");
		for (int i=0;i<TAM_INSTRUCAO;i++) {
				ler >> operacao[i].opcode;
			     ler>> operacao[i].endereco1.endBloco;
				 ler>> operacao[i].endereco1.endPalavra;
				 ler>> operacao[i].endereco2.endBloco;
				ler>> operacao[i].endereco2.endPalavra;
		}
		ler.close();
	//imprime_ram(ram,TAM_RAM,4);

	while (operacao[pc].opcode != -1){
		val1 = MMU(operacao[pc].endereco1,miss1,miss2,miss3,hit1,hit2,hit3, TC1, TC2, TC3, cache1, cache2, cache3, ram);
		val2 = MMU(operacao[pc].endereco2,miss1,miss2,miss3,hit1,hit2,hit3,TC1,TC2,TC3, cache1, cache2, cache3, ram);
		Maquina(operacao, pc,miss1,miss2,miss3,hit1,hit2,hit3, TC1, TC2, TC3, cache1, cache2, cache3, ram);

	/*	if (operacao[pc].opcode==0)
			cout<<val1<<"+"<<val2<<"="<<MMU(operacao[pc].endereco1,miss1,miss2,miss3,hit1,hit2,hit3,TC1,TC2,TC3, cache1,cache2,cache3,ram)<<endl;

		else if (operacao[pc].opcode==1)
			cout<<val1<<"-"<<val2<<"="<<MMU(operacao[pc].endereco1,miss1,miss2,miss3,hit1,hit2,hit3,TC1,TC2,TC3,cache1,cache2,cache3,ram)<<endl;

		else if(operacao[pc].opcode==2){
			multiplica(operacao,pc,miss1,miss2,miss3,hit1,hit2,hit3,TC1,TC2,TC3,cache1,cache2,cache3,ram);
			cout<<val1<<"x"<<val2<<"="<<MMU(operacao[pc].endereco1,miss1,miss2,miss3,hit1,hit2,hit3,TC1,TC2,TC3,cache1,cache2,cache3,ram)<<endl;
		}
	*/
		pc++;
	}
		Maquina(operacao,pc,miss1,miss2,miss3,hit1,hit2,hit3,TC1,TC2,TC3,cache1,cache2,cache3,ram);

	cout<<"CACHE HIT L1:"<<hit1<<endl;
	cout<<"CACHE MISS L1:"<<miss1<<endl;
	custo=hit1*100/(hit1+hit2+hit3+miss3);
	cout<<"Porcentagem:"<<custo<<endl;
	custo2=((TAM_INSTRUCAO*custo)/100)*10;
	cout<<"Custo cache 1:"<<custo2<< " de memoria"<<endl<<endl;
	
	cout<<"CACHE HIT L2:"<<hit2<<endl;
	cout<<"CACHE MISS L2:"<<miss2<<endl;
	custo=hit2*100/(hit1+hit2+hit3+miss3);
	cout<<"Porcentagem:"<<custo<<endl;
	custo2=((TAM_INSTRUCAO*custo)/100)*100;
	cout<<"Custo cache 2:"<<custo2<< " de memoria"<<endl<<endl;
	
	cout<<"CACHE HIT L3:"<<hit3<<endl;
	cout<<"CACHE MISS L3:"<<miss3<<endl;
	custo=hit3*100/(hit1+hit2+hit3+miss3);
	cout<<"Porcentagem:"<<custo<<endl;
	custo2=((TAM_INSTRUCAO*custo)/100)*1000;
	cout<<"Custo cache 3:"<<custo2<< " de memoria"<<endl<<endl;
	
	cout<<"RAM:"<<miss3<<endl;
	custo=miss3*100/(hit1+hit2+hit3+miss3);
	cout<<"Porcentagem:"<<custo<<endl;
	custo2=((TAM_INSTRUCAO*custo)/100)*10000;
	cout<<"Custo na Ram:"<<custo2<< " de memoria"<<endl<<endl;
	
	cout<<endl;
	//imprime_ram(ram,TAM_RAM,4);
	deleta_ram(ram,TAM_RAM);
	return 0;
}


//-------------------------------------------------FUNCOES DE ATRIBUIR VALORES E INSERIR--------------------------------------------------------------
void insere_enderecos(Instrucao operacao[TAM_INSTRUCAO]){
	srand(time(NULL));
	for(int i=0;i<TAM_INSTRUCAO;i++){
		operacao[i].opcode =rand() %2;
		operacao[i].endereco1.endBloco = rand() % 1000;
		operacao[i].endereco1.endPalavra = rand() % 4;
		operacao[i].endereco2.endBloco = rand() % 1000;
		operacao[i].endereco2.endPalavra = rand() % 4;
	}
	operacao[TAM_INSTRUCAO -1].opcode=-1;
}

void cria_ram(int**ram,int taml,int tamc){
	srand(time(NULL));
	for (int i=0;i<taml;i++) {
		ram[i]=new int[4];
		for (int j=0;j<tamc;j++)
			ram[i][j]=rand() % 10;
	}
}

void zera_cache(Linha *cache, int tam){
	for (int j=0;j<4;j++){
		for (int i=0;i<tam;i++){
			cache[i].palavras[j]= -1;
			cache[i].endBloco= -1;
			cache[i].atualiza=0;
			cache[i].cachehit=0;
		}
	}
}

void atribui_cache(int valor,Linha *cache,int tam,TEndereco end) {
	for (int i=0;i<tam;i++) {
		if(cache[i].endBloco==end.endBloco) {
			cache[i].palavras[end.endPalavra]=valor;
			cache[i].atualiza = 1;
		}
	}
}

void multiplica(Instrucao memoria[TAM_INSTRUCAO],int pc,int &miss1,int &miss2,int &miss3,int &hit1,int &hit2,int &hit3,int taml1,int taml2,int taml3,Linha *cache1,Linha *cache2,Linha *cache3,int **ram) {
	int  ValorUm,ValorDois;
	memoria[pc].opcode=1;
	ValorUm=MMU(memoria[pc].endereco1,miss1,miss2,miss3,hit1,hit2,hit3,taml1,taml2,taml3,cache1,cache2,cache3, ram);
	ValorDois=MMU(memoria[pc].endereco2,miss1,miss2,miss3,hit1,hit2,hit3,taml1,taml2,taml3,cache1,cache2,cache3, ram);
	atribui_cache(ValorUm,cache1,taml1,memoria[pc].endereco2);

	if(ValorUm>1)
		for (int i=0;i<ValorDois-1;i++)
			Maquina(memoria,pc,miss1,miss2,miss3,hit1,hit2,hit3,taml1,taml2,taml3,cache1,cache2,cache3,ram);

	else if(ValorDois==0)
		atribui_cache(0,cache1,taml1,memoria[0].endereco2);

	else if(ValorDois==1)
		atribui_cache(ValorUm,cache1,taml1,memoria[0].endereco2);
}

void atualiza_ram(int**ram,Linha *cache,int tam){
	for (int i=0;i<tam;i++){
		if (cache[i].atualiza==1){
			cache[i].cachehit=0;
			cache[i].atualiza=0;
			for(int j=0;j<4;j++){
				ram[cache[i].endBloco][j]=cache[i].palavras[j];
			}
			zera_cache(cache,tam);
		}
		else
			zera_cache(cache,tam);
	}
}


//--------------------------------------------------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------FUNCOES QUE IMPRIMEM E DELETAM----------------------------------------------------------//
void deleta_ram(int**ram,int tam){
	for (int i=0;i<tam;i++)
		delete ram[i];

	delete ram;
}

void imprime_ram(int**ram,int taml,int tamc){
	cout<<"\nRAM:"<<endl;
	for (int i=0;i<taml;i++){
		for (int j=0;j<tamc;j++) {
			cout<<ram[i][j]<<" ";
		}
		cout<<endl;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------//


//--------------------------------------------------FUNCOES QUE TROCAM,PEGAM,BUSCAM----------------------------------------------------------------------//
int busca_cache(TEndereco end, Linha *cache, int tam,int aux,int &j){
	for (j=0;j<tam;j++){
		if (cache[j].endBloco==end.endBloco){
			if(aux==1)
				return cache[j].palavras[end.endPalavra];
			else
				return 1;
		}
	}
	return -1;
}

void Troca_Caches(Linha *cache1,int tam,Linha *cache2){
	for(int i=0;i<tam;i++){
		if(cache2[i].atualiza==1){
			cache1[i].atualiza=cache2[i].atualiza;
			cache1[i].endBloco=cache2[i].endBloco;
			for(int j=0;j<4;j++){
				cache1[i].palavras[j]=cache2[i].palavras[j];
			}
		}	
	}
		zera_cache(cache1,tam);
}

void pega_da_Ram(int endBloco,int **ram,Linha *cache,int tam){
	int aux=0;
	for (int i=0;i<tam;i++) {
		if(cache[i].endBloco==-1){
			if(aux==0){
				aux++;
				for(int j=0;j<4;j++){
					cache[i].atualiza=0;
					cache[i].endBloco=endBloco;
					cache[i].palavras[j]=ram[endBloco][j];
					cache[i].cachehit=0;
				}
			}
		}
	}
	
	if(aux==0){
		int elimina=0;
		for (int i = 0; i < tam; i++){
			if (cache[elimina].cachehit > cache[i].cachehit)
				elimina = i;
		}

		if (cache[elimina].atualiza==1){
			for (int j=0; j<4;j++){
				Linha aux;
				aux.palavras[j] = cache[elimina].palavras[j];
				aux.endBloco = cache[elimina].endBloco;
				cache[elimina].palavras[j]=ram[endBloco][j];
				ram[aux.endBloco][j]=aux.palavras[j];
			}
		}
		cache[elimina].endBloco=endBloco;
		cache[elimina].cachehit=0;

		if (cache[elimina].atualiza == 0)
			for (int i=0;i<4;i++){
				cache[elimina].atualiza=0;
				cache[elimina].palavras[i]=ram[endBloco][i];
			}
		cache[elimina].atualiza=0;
	}
}

void pega_da_Cache(int endBloco, Linha *cache, int vai, Linha *origem, int vazo){
	int aux = 0;
	for (int i=0;i<vai;i++) {
		if (cache[i].endBloco==-1 && aux==0){
			aux++;
			for (int j=0;j<vazo;j++){
				if (origem[j].endBloco==endBloco){
					cache[i].endBloco=endBloco;
					cache[i].cachehit=0;
					cache[i].atualiza=origem[j].atualiza;
					origem[j].endBloco=-1;
					origem[j].cachehit=0;

					for (int l=0;l<4;l++) {
						if(origem[j].atualiza==1){
							cache[i].atualiza=1;
						}
						else
							cache[i].atualiza=0;
						cache[i].palavras[l]=origem[j].palavras[l];
						origem[i].palavras[l]=-1;
					}
				}
			}
		}
	}
	if(aux==0){
		int elimina=0;
		for(int i=0;i<vai;i++){
			if(cache[elimina].cachehit>cache[i].cachehit)
				elimina = i;
		}
		for(int l=0;l<vazo;l++){
			if(origem[l].endBloco==endBloco){
				Linha aux;
				aux.atualiza = cache[elimina].atualiza;
				aux.cachehit = cache[elimina].cachehit;
				aux.endBloco = cache[elimina].endBloco;
				cache[elimina].endBloco=endBloco;
				cache[elimina].cachehit=0;
				origem[l].endBloco = aux.endBloco;
				origem[l].cachehit =0;
				origem[l].atualiza = aux.atualiza;
				for (int j= 0;j<4;j++){
					aux.palavras[j]=cache[elimina].palavras[j];
					cache[elimina].palavras[j] = origem[l].palavras[j];
					origem[l].palavras[j] = aux.palavras[j];
				}
			}
		}
	}
}


//-------------------------------------------------FUNCOES PRINCIPAIS-------------------------------------------------------------------------------//

int MMU(TEndereco end,int &miss1,int &miss2,int &miss3,int &cache1hit,int &cache2hit,int &cache3hit,int taml1,int taml2,int taml3,Linha *cache1,Linha *cache2,Linha *cache3,int **ram){
	int i=0,j=0;
	i=busca_cache(end, cache1, taml1,1,j);
	if (i!=-1){
		cache1[j].cachehit++;
		cache1hit++;
		return i;//busca_cache(end, cache1, taml1,1,j);
	}
	miss1++;

	i=busca_cache(end, cache2, taml2,2,j);
	if (i!=-1){
		cache2[j].cachehit++;
		cache2hit++;
		pega_da_Cache(end.endBloco,cache1,taml1,cache2,taml2);
		return busca_cache(end,cache1,taml1,1,j);
	}
	miss2++;

	i=busca_cache(end,cache3,taml3,3,j);
	if(i!=-1){
		cache3[j].cachehit++;
		cache3hit++;
		pega_da_Cache(end.endBloco, cache2, taml2, cache3, taml3);
		pega_da_Cache(end.endBloco, cache1, taml1, cache2, taml2);
		return busca_cache(end, cache1, taml1,1,j);
	}
	miss3++;
	
	pega_da_Ram(end.endBloco, ram, cache3, taml3);
	pega_da_Cache(end.endBloco, cache2, taml2, cache3, taml3);
	pega_da_Cache(end.endBloco, cache1, taml1, cache2, taml2);
	return busca_cache(end, cache1, taml1,1,j);
}

void Maquina(Instrucao instrucao[TAM_INSTRUCAO],int pc,int&miss1,int&miss2,int&miss3,int &hit1, int &hit2, int &hit3,int taml1,int taml2,int taml3,Linha *cache1,Linha *cache2,Linha *cache3,int **ram){
	int soma,envia,subtracao;
	switch (instrucao[pc].opcode){

	case -1:
		atualiza_ram(ram,cache3,taml3);
		Troca_Caches(cache3,taml2,cache2);
		atualiza_ram(ram,cache3,taml3);
		Troca_Caches(cache2,taml1,cache1);
		Troca_Caches(cache3,taml2,cache2);
		atualiza_ram(ram,cache3,taml3);
		cout << "Finalizado!" << endl;
		break;

	case 0:
		soma=MMU(instrucao[pc].endereco1,miss1,miss2,miss3,hit1,hit2,hit3,taml1,taml2,taml3,cache1,cache2,cache3,ram)+MMU(instrucao[pc].endereco2,miss1,miss2,miss3,hit1,hit2,hit3,taml1, taml2, taml3,cache1,cache2,cache3, ram);
		envia=MMU(instrucao[pc].endereco1,miss1,miss2,miss3,hit1,hit2,hit3,taml1,taml2,taml3,cache1,cache2,cache3,ram);
		atribui_cache(soma,cache1, taml1,instrucao[pc].endereco1);
		break;

	case 1:
		subtracao = MMU(instrucao[pc].endereco1,miss1,miss2,miss3,hit1,hit2,hit3,taml1,taml2,taml3,cache1,cache2,cache3,ram) - MMU(instrucao[pc].endereco2,miss1,miss2,miss3,hit1,hit2,hit3,taml1,taml2,taml3,cache1,cache2,cache3,ram);
		envia = MMU(instrucao[pc].endereco1,miss1,miss2,miss3,hit1,hit2,hit3,taml1,taml2,taml3,cache1,cache2,cache3,ram);
		atribui_cache(subtracao,cache1,taml1,instrucao[pc].endereco1);

		break;

	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------//
