#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>

// Adicionei esse método porque não funciona no mac
char *strlwr(char *str)
{
    unsigned char *p = (unsigned char *)str;

    while (*p)
    {
        *p = tolower((unsigned char)*p);
        p++;
    }

    return str;
}

struct TMedico
{
    int codigo;
    char nome [50];
    char telefone [11];
    char especialidade [40];
};
typedef struct TMedico medico;

struct Tpaciente
{
    int codigo;
    char nome[70];
    char endereco[100];
    char telefone[9];
    char dia_nasc[2];
    char mes_nasc[2];
    char ano_nasc[4];
};
typedef struct Tpaciente paciente;

struct TConsulta
{
    int codigo_consulta;
    int codigo_medico;
    char nome_medico[50];
    int codigo_paciente;
    char nome_paciente[70];
    int dia;
    int mes;
    int ano;
    int hora;
    int minutos;
};
typedef struct TConsulta consulta;

void localiza_consultas(FILE * f)
{
    int posicao = -1, achou = 0;
    char nome[50];

    printf("Digite um nome: ");
    fflush(stdin);
    gets(nome);

    consulta con;
    fseek(f,0, SEEK_SET);
    fread(&con, sizeof(con), 1, f);

    while(!feof(f) && !achou)
    {
        posicao++;
        // compara com o nome do paciente
        if (strcmp(con.nome_paciente, nome) == 0)
        {
            printf("nome do médico: %s", con.nome_medico);
            printf("codigo consulta: %d", con.codigo_consulta);
            printf("data: %d/%d/%d", con.dia, con.mes, con.ano);

        }
        fread(&con, sizeof(con), 1, f);
    }

}

void cancelar_consulta()
{
    int codigo_c, opc;
    consulta consulta_lista;
    FILE *fc;
    fc = fopen("lista_consultas.dat", "r+b");
    if (fc == NULL)
    {
        fc = fopen("lista_consultas.dat", "w+b");
    }
    fseek(fc,0,SEEK_SET);
    if (feof(fc))
    {
        return;
    }
    printf("Qual o codigo da consulta que deseeja cancelar: ");
    scanf("%d",&codigo_c);

    int posi = -1;
    fseek(fc,0,SEEK_SET);
    fread(&consulta_lista, sizeof(consulta_lista), 1, fc);
    while(!feof(fc))
    {
        posi++;
        if (consulta_lista.codigo_consulta == codigo_c)
        {
            do
            {
                system("cls");
                printf("Consulta:\n");
                printf("| Código: %d | Médico: %s | Paciente: %s | Data: %d/%d/%d | Horário: %d:%d |\n", consulta_lista.codigo_consulta, consulta_lista.nome_medico, consulta_lista.nome_paciente,consulta_lista.dia,consulta_lista.mes,consulta_lista.ano,consulta_lista.hora,consulta_lista.minutos);
                printf("Deseja apagar essa consulta:\n1 - Sim\n2 -Não\n--> ");
                scanf("%d",&opc);
                if (opc == 1||opc == 2)
                {
                    if (opc == 1)
                    {
                        consulta_lista.codigo_consulta = consulta_lista.codigo_consulta*(-1);
                        consulta_lista.dia = consulta_lista.dia*(-1);
                        rewind(fc);
                        fseek(fc,sizeof(consulta_lista)*posi,SEEK_SET);
                        fwrite(&consulta_lista,sizeof(consulta_lista),1,fc);
                        fflush(fc);
                        fclose(fc);
                        printf("\nConsulta cancelada com sucesso.\n");
                    }
                }
                else
                {
                    printf("\nResposta inválida, tente de novo.\n");
                    system("pause");
                }
            }
            while(opc!=1&&opc!=2);
            return;
        }
        fread(&consulta_lista,sizeof(consulta_lista),1,fc);
        if (feof(fc))
        {
            printf("\nConsulta não cadastrada.\n\n");
            return;
        }
    }
}

int achar_novo_codigo(FILE *fc)
{
    int novo_codigo;
    consulta ultima_consulta;
    fseek(fc,0,SEEK_SET);
    fread(&ultima_consulta, sizeof(ultima_consulta),1, fc);
    novo_codigo = 15000;
    if(!feof(fc))
    {
        while (!feof(fc))
        {
            fread(&ultima_consulta, sizeof(ultima_consulta),1, fc);
            if (feof(fc))
            {
                if (ultima_consulta.codigo_consulta < 0)
                {
                    novo_codigo = (ultima_consulta.codigo_consulta - 1)*(-1);
                }
                else
                {
                    novo_codigo = ultima_consulta.codigo_consulta + 1;
                }
                return novo_codigo;
            }
        }
    }
    else
    {
        return novo_codigo;
    }
    return novo_codigo;
}

int pegar_codigo_paciente(char nome[50])
{
    FILE *fp;
    int achou = 0;
    strlwr(nome);
    paciente p;
    fp = fopen("lista_pacientes.dat", "r+b");
    fseek(fp,0,SEEK_SET);
    fread(&p,sizeof(p),1,fp);
    while (!feof(fp)&&!achou)
    {
        strlwr(p.nome);
        if (!strcmp(p.nome, nome))
        {
            achou = 1;
            return p.codigo;
        }
        fread(&p,sizeof(p),1,fp);
    }
    return -1;
}

int pegar_codigo_medico(char nome[50])
{
    FILE *fm;
    int achou = 0;
    strlwr(nome);
    medico m;
    fm = fopen("lista_medicos.dat", "r+b");
    fseek(fm,0,SEEK_SET);
    fread(&m,sizeof(m),1,fm);
    while (!feof(fm)&&!achou)
    {
        strlwr(m.nome);
        if (!strcmp(m.nome, nome))
        {
            achou = 1;
            return m.codigo;
        }
        fread(&m,sizeof(m),1,fm);
    }
    return -1;
}

void listar_consultas_por_medico()
{
    int v = 0;
    char nome_medico[50];
    consulta consulta_lista;
    FILE *fc;
    fc = fopen("lista_consultas.dat", "a+b");
    fseek(fc,0,SEEK_SET);
    fread(&consulta_lista,sizeof(consulta_lista),1,fc);
    printf("Digite o nome do médico que deseja listar suas consultas: ");
    fflush(stdin);
    gets(nome_medico);
    printf("\nLista de consultas do médico %s:\n",nome_medico);
    if (feof(fc))
    {
        printf("\nLista vazia.\n\n");
    }
    while(!feof(fc))
    {
        if (consulta_lista.codigo_consulta > 0 && !strcmpi(consulta_lista.nome_medico,nome_medico))
        {
            v++;
            printf("| Código: %5d | Médico: %20s | Paciente: %20s | Data: %02d/%02d/%d | Horário: %02d:%02d |\n", consulta_lista.codigo_consulta, consulta_lista.nome_medico, consulta_lista.nome_paciente,consulta_lista.dia,consulta_lista.mes,consulta_lista.ano,consulta_lista.hora,consulta_lista.minutos);
        }
        fread(&consulta_lista,sizeof(consulta_lista),1,fc);
        if(feof(fc)&&v == 0)
        {
            printf("\nLista vazia.\n\n");
        }
    }
    printf("------------------------------------------------------------------------------------------------------------------------\n");
}

void listar_consultas_por_data()
{
    int v = 0,dia,mes,ano;
    consulta consulta_lista;
    FILE *fc;
    fc = fopen("lista_consultas.dat", "a+b");
    fseek(fc,0,SEEK_SET);
    fread(&consulta_lista,sizeof(consulta_lista),1,fc);
    printf("Digite a data que deseja filtrar as consultas: ");
    printf("\nInsira a data da consulta:\n");

    do
    {
        printf("Dia: ");
        scanf("%d",&dia);
        if (dia>31||dia<1)
        {
            printf("Dia inválido, tente de novo:\n");
        }
    }
    while (dia>31||dia<1);

    do
    {
        printf("Mês: ");
        scanf("%d",&mes);
        if (mes>12||dia<1)
        {
            printf("Mês inválido, tente de novo:\n");
        }
    }
    while (mes>12||dia<1);

    do
    {
        printf("Ano: ");
        scanf("%d",&ano);
        if (ano<2019)
        {
            printf("Ano inválido, tente de novo:\n");
        }
    }
    while (ano<2019);

    printf("\nConsultas do dia %02d/%02d/%04d:\n",dia,mes,ano);

    if (feof(fc))
    {
        printf("\nLista vazia.\n\n");
    }
    while(!feof(fc))
    {
        if (consulta_lista.codigo_consulta > 0 && dia ==consulta_lista.dia && mes == consulta_lista.mes && ano == consulta_lista.ano)
        {
            v++;
            printf("| Código: %5d | Médico: %20s | Paciente: %20s | Data: %02d/%02d/%d | Horário: %02d:%02d |\n", consulta_lista.codigo_consulta, consulta_lista.nome_medico, consulta_lista.nome_paciente,consulta_lista.dia,consulta_lista.mes,consulta_lista.ano,consulta_lista.hora,consulta_lista.minutos);
        }
        fread(&consulta_lista,sizeof(consulta_lista),1,fc);
        if(feof(fc)&&v == 0)
        {
            printf("\nLista vazia.\n\n");
        }
    }
    printf("------------------------------------------------------------------------------------------------------------------------\n");
}

int listar_consultas()
{
    consulta consulta_lista;
    int v = 0;
    FILE *fc;
    fc = fopen("lista_consultas.dat", "a+b");
    fseek(fc,0,SEEK_SET);
    fread(&consulta_lista,sizeof(consulta_lista),1,fc);
    printf("Lista de consultas:\n");
    if (feof(fc))
    {
        printf("\nLista vazia.\n\n");
        return -1;
    }
    while(!feof(fc))
    {
        if (consulta_lista.codigo_consulta > 0)
        {
            v++;
            printf("| Código: %5d | Médico: %20s | Paciente: %20s | Data: %02d/%02d/%d | Horário: %02d:%02d |\n", consulta_lista.codigo_consulta, consulta_lista.nome_medico, consulta_lista.nome_paciente,consulta_lista.dia,consulta_lista.mes,consulta_lista.ano,consulta_lista.hora,consulta_lista.minutos);
        }
        fread(&consulta_lista,sizeof(consulta_lista),1,fc);
    }
    if (v == 0)
    {
        printf("\nLista vazia.\n\n");
        return -1;
    }
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    return 0;
}

int verificar_horario(int dia, int mes, int ano, int hora, int minutos)
{
    int verifica_horario = 2;
    FILE *fc;
    consulta verificacao;
    fc = fopen("lista_consultas.dat", "r+b");
    fseek(fc,0,SEEK_SET);
    fread(&verificacao, sizeof(verificacao),1, fc);
    if (feof(fc))
    {
        return 0;
    }
    while (!feof(fc))
    {
        if (dia == verificacao.dia && mes == verificacao.mes && ano == verificacao.ano)
        {
            if(hora==verificacao.hora)
            {
                if(abs(minutos - verificacao.minutos) < 30)
                {
                    verifica_horario = 0;
                }
                else
                {
                    if (verifica_horario == 2)
                    {
                        verifica_horario = 1;
                    }
                }
            }
            else
            {
                if(hora>verificacao.hora)
                {
                    if(minutos + (60 - verificacao.minutos) < 30)
                    {
                        verifica_horario = 0;
                    }
                    else
                    {
                        if (verifica_horario == 2)
                        {
                            verifica_horario = 1;
                        }
                    }
                }
                else
                {
                    if(verificacao.minutos + (60 - minutos) < 30)
                    {
                        verifica_horario = 0;
                    }
                    else
                    {
                        if (verifica_horario == 2)
                        {
                            verifica_horario = 1;
                        }
                    }
                }
            }
        }
        fread(&verificacao,sizeof(verificacao),1,fc);
        if (feof(fc)&&verifica_horario==2)
        {
            verifica_horario = 1;
        }
    }
    if (verifica_horario == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void cadastrar_consulta()
{
    int verifica_horario, opc;
    FILE *fc;
    fc = fopen("lista_consultas.dat", "r+b");
    if (fc == NULL)
    {
        fc = fopen("lista_consultas.dat", "w+b");
    }
    consulta nova_consulta, verificacao;
    char nome_paciente[70], nome_medico[50];

    do
    {
        printf("\nDigite o nome completo do paciente: ");
        fflush(stdin);
        gets(nome_paciente);
        nova_consulta.codigo_paciente = pegar_codigo_paciente(nome_paciente);
        if (nova_consulta.codigo_paciente == -1)
        {
            printf("\nPaciente não encontrado, tente de novo.\n\n");
            printf("1 - Tentar de novo\n2 - Cancelar consulta\n-->");
            scanf("%d",&opc);
            if (opc == 2)
            {
                return;
            }
        }
    }
    while(nova_consulta.codigo_paciente == -1);
    strcpy(nova_consulta.nome_paciente, nome_paciente);

    do
    {
        printf("\nDigite o nome completo de Médico: ");
        fflush(stdin);
        gets(nome_medico);
        nova_consulta.codigo_medico = pegar_codigo_medico(nome_medico);
        if (nova_consulta.codigo_medico == -1)
        {
            printf("\nMédico não encontrado, tente de novo.\n\n");
            printf("1 - Tentar de novo\n2 - Cancelar consulta\n-->");
            scanf("%d",&opc);
            if (opc == 2)
            {
                return;
            }
        }
    }
    while(nova_consulta.codigo_medico == -1);
    strcpy(nova_consulta.nome_medico, nome_medico);

    int verificar_medico = 0;

    do
    {
        printf("\nInsira a data da consulta:\n");
        do
        {
            printf("Dia: ");
            scanf("%d",&nova_consulta.dia);
            if (nova_consulta.dia>31||nova_consulta.dia<1)
            {
                printf("Dia inválido, tente de novo:\n");
            }
        }
        while (nova_consulta.dia>31||nova_consulta.dia<1);

        do
        {
            printf("Mês: ");
            scanf("%d",&nova_consulta.mes);
            if (nova_consulta.mes>12||nova_consulta.dia<1)
            {
                printf("Mês inválido, tente de novo:\n");
            }
        }
        while (nova_consulta.mes>12||nova_consulta.dia<1);

        do
        {
            printf("Ano: ");
            scanf("%d",&nova_consulta.ano);
            if (nova_consulta.ano<2019)
            {
                printf("Ano inválido, tente de novo:\n");
            }
        }
        while (nova_consulta.ano<2019);


        fseek(fc,0,SEEK_SET);
        fread(&verificacao, sizeof(verificacao),1, fc);
        verificar_medico = 0;
        if (!feof(fc))
        {
            while (!feof(fc))
            {
                if (nova_consulta.dia == verificacao.dia&&nova_consulta.mes == verificacao.mes&&nova_consulta.ano == verificacao.ano&&nova_consulta.codigo_medico == verificacao.codigo_medico)
                {
                    verificar_medico++;
                }
                fread(&verificacao,sizeof(verificacao),1,fc);
            }
        }
        else
        {
            verificar_medico = 0;
        }

        if(verificar_medico<2)
        {
            do
            {
                printf("\nInsira o horário da consulta:\n");
                do
                {
                    printf("Hora: ");
                    scanf("%d",&nova_consulta.hora);
                    if (nova_consulta.hora>=24||nova_consulta.hora<0)
                    {
                        printf("Hora inválido, tente de novo:\n");
                    }
                }
                while (nova_consulta.hora>=24||nova_consulta.hora<0);

                do
                {
                    printf("Minutos: ");
                    scanf("%d",&nova_consulta.minutos);
                    if (nova_consulta.minutos>=60||nova_consulta.minutos<0)
                    {
                        printf("Minutos inválidos, tente de novo:\n");
                    }
                }
                while (nova_consulta.minutos>=60||nova_consulta.minutos<0);

                verifica_horario = verificar_horario(nova_consulta.dia, nova_consulta.mes, nova_consulta.ano, nova_consulta.hora, nova_consulta.minutos);
                if (verifica_horario == 1)
                    printf("Horário ocupado, tente novamente.\n");
            }
            while (verifica_horario);

            nova_consulta.codigo_consulta = achar_novo_codigo(fc);

            system("cls");
            printf("Consulta cadastrada com susseso:\n");
            printf("| Código: %5d | Médico: %20s | Paciente: %20s | Data: %02d/%02d/%d | Horário: %02d:%02d |\n", nova_consulta.codigo_consulta, nome_medico, nome_paciente,nova_consulta.dia,nova_consulta.mes,nova_consulta.ano,nova_consulta.hora,nova_consulta.minutos);

            fseek(fc,0,SEEK_END);
            fwrite(&nova_consulta, sizeof(nova_consulta),1,fc);
        }
        else
        {
            printf("Esse médico já tem duas consultas marcadas nesse dia. Tente outro dia.\n");
        }
    }
    while (verificar_medico >2);
    fclose(fc);
}

void menu_consultas()
{
    setlocale(LC_ALL, "portuguese");
    int opc;
    do
    {
        system("cls");
        printf("--------------------------\n");
        printf("|        Escolha:        |\n");
        printf("--------------------------\n");
        printf("| 1 - Cadastrar consulta |\n");
        printf("| 2 - Cancelar consulta  |\n");
        printf("| 3 - Listar consultas   |\n");
        printf("| 4 - Listar por paciente|\n");
        printf("| 5 - Listar por medicos |\n");
        printf("| 6 - Listar por data    |\n");
        printf("| 7 - Voltar             |\n");
        printf("--------------------------\n\n--> ");
        scanf("%d",&opc);
        switch (opc)
        {
        case 1:
            system("cls");
            cadastrar_consulta();
            system("pause");
            system("cls");
            break;
        case 2:
            if (!listar_consultas())
                cancelar_consulta();
            system("pause");
            system("cls");
            break;
        case 3:
            system("cls");
            listar_consultas();
            system("pause");
            system("cls");
            break;
        case 4:
            system("cls");
            //localiza_consultas(fopen("lista_consultas.dat", "a+b"));
            system("pause");
            system("cls");
            break;
        case 5:
            system("cls");
            listar_consultas_por_medico();
            system("pause");
            system("cls");
            break;
        case 6:
            system("cls");
            listar_consultas_por_data();
            system("pause");
            system("cls");
            break;
        case 7:
            break;
        default:
            printf("Número inválido, tente de novo.\n");
            system("pause");
            system("cls");
            break;
        }
    }
    while(opc!=7);
    return;
}

int localiza_codigo_medico(FILE *f, int cod, char nome_medico[50])
{
    int posicao = -1, achou = 0;
    medico m;
    fseek(f, 0, SEEK_SET);
    fread(&m, sizeof(m), 1, f);
    while (!feof(f) && !achou)
    {
        posicao++; // semelhante a posicao = posicao +1;
        if (m.codigo == cod || strcmp(m.nome, nome_medico) == 0)
        {
            achou = 1;
        }
        fread(&m, sizeof(m),1, f);
    }
    if (achou)
    {
        return posicao;
    }
    else
    {
        return -1;
    }

}

void cadastro_medico (FILE *f)
{
    medico m;

    int posicao;

    printf("Digite um código: ");
    fflush(stdin);

    scanf("%d", &m.codigo);

    posicao = localiza_codigo_medico(f, m.codigo, m.nome);
    if (posicao == -1) // não tinha codigo no arquivo
    {
        printf("Digite um nome: ");
        fflush(stdin);
        gets(m.nome);

        printf("\nDigite a especialidade médica em letras maiúsculas e sem acentos: ");
        fflush(stdin);
        gets(m.especialidade);

        printf("\nPara o telefone, digite 9 números, sem espaço entre eles");
        printf("Digite um telefone: ");
        fflush(stdin);
        gets(m.telefone);


        fseek(f,0,SEEK_END); // posicionado o arquivo no fnal
        fwrite(&m, sizeof(m), 1, f); //gravando os dados
        fflush(f);
    }

}

int localiza_codigo_paciente(FILE *f, int cod, char nome_paciente[50]) // procura no arquivo se já existe o paciente, pelo código informado
{
    int posicao=-1,achou=0;
    paciente p;
    fseek(f,0,SEEK_SET);
    fread(&p, sizeof(p),1, f);
    while (!feof(f) && !achou)
    {
        posicao++;
        if (p.codigo == cod || strcmp(p.nome, nome_paciente) == 0)
        {
            achou=1;
        }
        fread(&p, sizeof(p), 1, f);
    }
    if (achou)
    {
        return posicao;
    }
    else
    {
        return -1;
    }
}

int localiza_medico_especialidade(FILE *f)
{
    char especialidade[40];
    printf("Digite uma especialidade para busca: ");
    fflush(stdin);
    gets(especialidade);
    int posicao=-1,achou=0;
    medico m;
    fseek(f,0,SEEK_SET);
    fread(&m, sizeof(m),1, f);
    printf("\n Médicos especializados em %s:\n", especialidade);
    while (!feof(f))
    {
        posicao++;
        if (strcmp(m.especialidade,especialidade) == 0)
        {
            printf("Código....:%d \n", m.codigo);
            printf("Nome.:%s \n", m.nome);
            printf("Telefone.....:%s\n", m.telefone);
            printf("Especialidade.....:%s\n", m.especialidade);
            achou = 1;
        }
        fread(&m, sizeof(m), 1, f);
    }
    if (achou)
    {
        return 0;
    }
    else
    {
        printf("\nEspecialidade não encontrada");
        return -1;
    }
}

int localiza_medico_nome(FILE * f)
{
    char nome[40];
    printf("Digite o nome do médico para busca: ");
    fflush(stdin);
    gets(nome);

    int posicao = -1, achou = 0;
    medico m;
    fseek(f,0,SEEK_SET);
    fread(&m, sizeof(m), 1, f);

    while (!feof(f))
    {
        posicao++;
        if (strcmp(m.nome, nome) == 0)
        {
            printf("Código....:%d \n", m.codigo);
            printf("Nome.:%s \n", m.nome);
            printf("Telefone.....:%s\n", m.telefone);
            printf("Especialidade.....:%s\n", m.especialidade);
            achou = 1;
        }
        fread(&m, sizeof(m), 1, f);
    }
    if (achou)
    {
        return 0;
    }
    else
    {
        printf("\nEspecialidade não encontrada");
        return -1;
    }
}

void listar_medicos()
{
    medico m;
    FILE *fm;
    fm = fopen("lista_medicos.dat", "a+b");
    fseek(fm,0,SEEK_SET);
    fread(&m,sizeof(m),1,fm);
    printf("Lista de médicos:\nzn");
    if (feof(fm))
    {
        printf("Lista vazia.\n\n");
    }
    while(!feof(fm))
    {
        if (m.codigo != -1)
        {
            printf("| Médico: %s | Codigo:%d | Especialidade:%s | Telefone: %s |\n", m.nome, m.codigo, strupr(m.especialidade), m.telefone);
        }
        else
        {
            printf("\nLista vazia.\n\n");
        }
        fread(&m,sizeof(m),1,fm);

    }
    printf("------------------------------------------------------------------------------------------------------------------------\n");
}

void menu_medicos()
{
    FILE * fm;
    fm = fopen("lista_medicos.dat", "a+b");
    int opc;
    do
    {
        system("cls");
        printf("----------------------------------------\n");
        printf("|               Escolha:               |\n");
        printf("----------------------------------------\n");
        printf("| 1 - Cadastrar médico                 |\n");
        printf("| 2 - Listar médicos por nome          |\n");
        printf("| 3 - Listar médicos por especialidade |\n");
        printf("| 4-  Listar todos os médicos          |\n");
        printf("| 5 - Voltar                           |\n");
        printf("----------------------------------------\n\n--> ");
        scanf("%d", &opc);
        switch (opc)
        {
        case 1:
            cadastro_medico(fm);
            system("pause");
            system("cls");
            break;
        case 2:
            localiza_medico_nome(fm);
            system("pause");
            system("cls");
            break;
        case 3:
            localiza_medico_especialidade(fm);
            system("pause");
            system("cls");
            break;
        case 4:
            system("cls");
            listar_medicos();
            system("pause");
            system("cls");
            break;

        case 5:
            break;
        default:
            printf("Número inválido, tente de novo.\n");
            system("pause");
            system("cls");
            break;
        }
    }
    while(opc!=5);
    fclose(fm);
    return;
}

void cadastro_pacientes (FILE *f)
{
    paciente p;
    int posicao; // Lendo os dados do teclado
    printf("Digite um código: ");
    fflush(stdin);
    scanf("%d", &p.codigo);
    posicao = localiza_codigo_paciente(f, p.codigo, p.nome);

    if (posicao == -1) // não tinha codigo no arquivo
    {
        printf("Digite o nome completo do paciente: ");
        fflush(stdin);
        gets(p.nome);

        printf("Ex.: R./Av. das Flores 105 APTO 402 Bloco 3");
        printf("Digite um endereço: ");
        fflush(stdin);
        gets(p.endereco);

        printf("\nPara o telefone, digite 9 números, sem espaço entre eles");
        printf("Digite um telefone: ");
        fflush(stdin);
        gets(p.telefone);

        printf("\nPara o dia, digite 2 números, sem espaço entre eles (Ex.: 06)");
        printf("Digite o dia do nascimento: ");
        fflush(stdin);
        gets(p.dia_nasc);

        printf("\nPara o mês, digite 2 números, sem espaço entre eles (Ex.: 01)");
        printf("Digite o mês do nascimento: ");
        fflush(stdin);
        gets(p.mes_nasc);

        printf("\nPara o ano, digite 4 números, sem espaço entre eles (Ex.: 1999)");
        printf("Digite o ano do nascimento: ");
        fflush(stdin);
        gets(p.ano_nasc);

        fseek(f, 0, SEEK_END); // posicionado o arquivo no final
        fwrite(&p, sizeof(p), 1, f); // gravando os dados na pasta
        fflush(f);

    }
    else
    {
        printf("Paciente %s já existe no arquivo. Inclusão não realiiada!\n", p.nome); // caso o paciente já exista no sistema - localiza por código e informa o nome
    }
}

void menu_pacientes()
{
    FILE *fp;
    fp = fopen("lista_pacientes.dat", "a+b");
    int opc;
    do
    {
        system("cls");
        printf("--------------------------\n");
        printf("|        Escolha:        |\n");
        printf("--------------------------\n");
        printf("| 1 - Cadastrar paciente |\n");
        printf("| 2 - Voltar             |\n");
        printf("--------------------------\n\n--> ");
        scanf("%d",&opc);
        switch (opc)
        {
        case 1:
            cadastro_pacientes(fp);
            system("pause");
            system("cls");
            break;
        case 2:
            system("cls");
            break;
        default:
            printf("Número inválido, tente de novo.\n");
            system("pause");
            system("cls");
            break;
        }
    }
    while(opc!=2);
    fclose(fp);
    return;
}

int main()
{
    FILE *fc, *fp, *fm;

    fc = fopen("lista_consultas.dat", "r+b");
    if (fc == NULL)
    {
        fc = fopen("lista_consultas.dat", "w+b");
    }
    fclose(fc);

    fp = fopen("lista_pacientes.dat", "r+b");
    if (fp == NULL)
    {
        fp = fopen("lista_pacientes.dat", "w+b");
    }
    fclose(fp);

    fm = fopen("lista_medicos.dat", "r+b");
    if (fm == NULL)
    {
        fm = fopen("lista_medicos.dat", "w+b");
    }
    fclose(fm);

    setlocale(LC_ALL, "portuguese");
    int opc;
    do
    {
        system("cls");
        printf("--------------------------\n");
        printf("|        Escolha:        |\n");
        printf("--------------------------\n");
        printf("| 1 - Consultas          |\n");
        printf("| 2 - Médicos            |\n");
        printf("| 3 - Pacientes          |\n");
        printf("| 4 - Sair               |\n");
        printf("--------------------------\n\n--> ");
        scanf("%d",&opc);

        switch (opc)
        {
        case 1:
            menu_consultas();
            break;
        case 2:
            menu_medicos();
            break;
        case 3:
            menu_pacientes();
            break;
        case 4:
            break;
        default:
            printf("Número inválido, tente de novo.\n");
            system("pause");
            system("cls");
            break;
        }
    }
    while(opc!=4);
    return 0;
}
