#include <stdio.h>
#include <string.h>

struct Conta {
    int numero;
    char titular[50];
    float saldo;
};

// gerar numero resumido
int genNum(FILE *arq) {
    struct Conta c;
    int ult = 0;

    rewind(arq);
    while (fread(&c, sizeof(struct Conta), 1, arq) == 1) {
        ult = c.numero;
    }
    return ult + 1;
}

// buscar conta resumido
int buscaC(FILE *arq, int num, struct Conta *out, long *pos) {
    struct Conta c;
    rewind(arq);
    *pos = -1;

    while (fread(&c, sizeof(struct Conta), 1, arq) == 1) {
        if (c.numero == num) {
            *out = c;
            *pos = ftell(arq) - sizeof(struct Conta);
            return 1;
        }
    }
    return 0;
}

int main() {
    FILE *arq = fopen("contas.dat", "r+b");
    if (arq == NULL) {
        arq = fopen("contas.dat", "w+b");
    }

    struct Conta c, origem, destino;
    long p1, p2;
    int op, n1, n2;
    float val;

    char seAdm[20] = "5204";  // senha admin
    char seIn[20];            // senha digitada

    do {
        printf("\nBem vindo ao banco de ITAMBACURI\n");
        printf("1 - Criar conta\n");
        printf("2 - Extrato\n");
        printf("3 - Deposito\n");
        printf("4 - Saque\n");
        printf("5 - Transferencia\n");
        printf("6 - Listar contas\n");
        printf("7 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &op);

        // Criar conta
        if (op == 1) {
            c.numero = genNum(arq);
            c.saldo = 0;

            printf("Nome do titular: ");
            scanf(" %[^\n]", c.titular);

            fseek(arq, 0, SEEK_END);
            fwrite(&c, sizeof(struct Conta), 1, arq);

            printf("Conta criada! Numero: %d\n", c.numero);
        }

        // Extrato
        else if (op == 2) {
            printf("Numero da conta: ");
            scanf("%d", &n1);

            if (buscaC(arq, n1, &c, &p1)) {
                printf("Titular: %s\n", c.titular);
                printf("Saldo: R$ %.2f\n", c.saldo);
            } else {
                printf("Conta nao encontrada.\n");
            }
        }

        // Depósito
        else if (op == 3) {
            printf("Numero da conta: ");
            scanf("%d", &n1);

            printf("Valor do deposito: ");
            scanf("%f", &val);

            if (buscaC(arq, n1, &c, &p1)) {
                c.saldo += val;
                fseek(arq, p1, SEEK_SET);
                fwrite(&c, sizeof(struct Conta), 1, arq);
                printf("Deposito realizado!\n");
            } else {
                printf("Conta nao encontrada.\n");
            }
        }

        // Saque
        else if (op == 4) {
            printf("Numero da conta: ");
            scanf("%d", &n1);

            printf("Valor do saque: ");
            scanf("%f", &val);

            if (buscaC(arq, n1, &c, &p1)) {
                if (c.saldo >= val) {
                    c.saldo -= val;
                    fseek(arq, p1, SEEK_SET);
                    fwrite(&c, sizeof(struct Conta), 1, arq);
                    printf("Saque realizado!\n");
                } else {
                    printf("Saldo insuficiente.\n");
                }
            } else {
                printf("Conta nao encontrada.\n");
            }
        }

        // Transferência
        else if (op == 5) {
            printf("Conta origem: ");
            scanf("%d", &n1);

            printf("Conta destino: ");
            scanf("%d", &n2);

            printf("Valor da transferencia: ");
            scanf("%f", &val);

            if (!buscaC(arq, n1, &origem, &p1)) {
                printf("Conta origem nao encontrada.\n");
                continue;
            }
            if (!buscaC(arq, n2, &destino, &p2)) {
                printf("Conta destino nao encontrada.\n");
                continue;
            }

            if (origem.saldo < val) {
                printf("Saldo insuficiente.\n");
                continue;
            }

            origem.saldo -= val;
            destino.saldo += val;

            fseek(arq, p1, SEEK_SET);
            fwrite(&origem, sizeof(struct Conta), 1, arq);

            fseek(arq, p2, SEEK_SET);
            fwrite(&destino, sizeof(struct Conta), 1, arq);

            printf("Transferencia concluida!\n");
        }

        // Listagem admin
        else if (op == 6) {
            printf("Digite a senha: ");
            scanf("%s", seIn);

            if (strcmp(seIn, seAdm) != 0) {
                printf("Senha incorreta.\n");
                continue;
            }

            rewind(arq);
            printf("\nCONTAS - BANCO DE ITAMBACURI\n");

            while (fread(&c, sizeof(struct Conta), 1, arq) == 1) {
                printf("Conta %d | Titular: %s | Saldo: R$ %.2f\n",
                       c.numero, c.titular, c.saldo);
            }
        }

    } while (op != 7);

    fclose(arq);
    return 0;
}
