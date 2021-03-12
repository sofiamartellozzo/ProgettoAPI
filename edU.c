#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#define lungh_testo 250

typedef enum
{
    false,
    true
} boolean;

/* struttura per ogni singola stringa */
typedef struct node
{
    char Array[lungh_testo];
    struct node *next;
    struct node *prec;

} t_node;

/* lista di comandi */
typedef struct command
{
    char istruzione;                        /* carattere per identificare l'istruzione */
    long int ind1, ind2, mod_count;         /* indici delle righe modificate */
    t_node *nuova_prima, *nuova_ultima;     /* puntatori alla nuova prima e ultima riga modificate */
    t_node *vecchia_prima, *vecchia_ultima; /* puntatori alla prima e ultima riga modificata */
    struct command *next;                   /* prossimo comando */
    struct command *prec;                   /* comando precedente */
} t_command;

/* sottofunzioni */
void Change(char *istr);            /* per gestire il comando c = change */
void Print(char *istr);             /* per gestire il comando p = printg */
void Delate(char *istr);            /* per gestire il comando d = delate */
void Undo(long int undo);           /* per gestire il comando u = undo */
void Redo(long int redo);           /* per gestire il comando r = redo */
void SvuotaLista();                 /* per cancellare la lista di comandi quando non sono più possibili undo, dopo un comando di change o delate */


long int count = 0;                  /* numero frasi */
t_node *first_line = NULL;           /* prima stringa */
t_node *last_line = NULL;            /* ultima stringa */
t_command *newest_command = NULL;    /* primo comando */
t_command *current_command = NULL;   /* puntatore per tener traccia del comando attuale */
t_command *oldest_command = NULL;    /* ultimo comando */
long int max_undo = 0;               /* numero di undo, in base al numero di comandi svolti */
long int max_redo = 0;               /* numero di redo, in base al numero di undo svolti*/

int main()
{
    boolean Fine = false;
    boolean Trovato = false;
    char Istruzione[100];   /* dove inserisco il comando letto */ 
    /* per ottimizzare i tempi raggruppo il numero di undo e redo per svolgerli insieme direttamente e non in modo prettamente sequenziale */
    long int zeta = 0;      /* per calcolo numero di undo */
    long int esse = 0;      /* per calcolo numero di redo */
    long int zeta2 = 0;     /* variabile ausiliaria per calcolo di undo */
    long int esse2 = 0;     /* variabile ausiliaria per calcolo di redo */
    char punto[3];

    while (Fine == false)
    {
        fgets(Istruzione, 100, stdin); 

        for (int i = 0; ((Trovato == false) && (i < 100)); i++)
        {
            if (Istruzione[i] == 'c')
            {
                if (zeta < esse){
                    esse = esse - zeta;
                    Redo(esse);
                }
                else if (zeta > esse){
                    zeta = zeta - esse;
                    Undo(zeta);
                }
                SvuotaLista();
                Change(Istruzione);
                fgets(punto, 3, stdin); //prendo il punto di fine comando 
                max_undo++;
                max_redo = 0;
                esse = 0;
                zeta = 0;
                Trovato = true;
            }
            if (Istruzione[i] == 'p')
            {

                if (zeta < esse){
                    esse = esse - zeta;
                    Redo(esse);
                }
                else if (zeta > esse){
                    zeta = zeta - esse;
                    Undo(zeta);
                }
                Print(Istruzione);
                esse= 0;
                zeta = 0;
                Trovato = true;
            }
            if (Istruzione[i] == 'd')
            {
                if (zeta < esse){
                    esse = esse - zeta;
                    Redo(esse);
                }
                else if (zeta > esse){
                    zeta = zeta - esse;
                    Undo(zeta);
                }
                SvuotaLista();
                Delate(Istruzione);
                max_undo++;
                max_redo = 0;
                esse = 0;
                zeta = 0;
                Trovato = true;
            }
            if (Istruzione[i] == 'u')
            {
                zeta2 = strtoimax(strtok(Istruzione, "u"), NULL, 10);
                if (zeta2 > max_undo){
                    zeta2 = max_undo;
                }
                zeta += zeta2;
                Trovato = true;
                max_redo += zeta2;
                max_undo -= zeta2;
            }
            if (Istruzione[i] == 'r')
            {
                esse2 = strtoimax(strtok(Istruzione, "r"), NULL, 10);
                if (esse2 > max_redo){
                    esse2 = max_redo;
                }
                esse += esse2;
                Trovato = true;
                max_redo -= esse2;
                max_undo += esse2;
            }
            if (Istruzione[i] == 'q')
            {
                Trovato = true;
                Fine = true;
            }
        }
        Trovato = false;
    }
    return 0;
}

void Change(char *istr)
{

    long int rig1 = strtoimax(strtok(istr, ","), NULL, 10);
    long int rig2 = strtoimax(strtok(NULL, "c"), NULL, 10);
    long int nrig = rig2 - rig1 + 1;

    t_command *new_command = malloc(sizeof(t_command));
    new_command->istruzione = 'c';
    new_command->ind1 = rig1;
    new_command->ind2 = rig2;
    new_command->mod_count = 0;
    new_command->next = NULL;
    new_command->prec = newest_command;
    if (newest_command != NULL)
    {
        new_command->prec->next = new_command;
    }
    else{
        /* primo comando */
        oldest_command = new_command;
    }
    newest_command = new_command;
    current_command = new_command;

    /* leggi la prima riga delle nuove righe */
    t_node *nuova_riga = malloc(sizeof(t_node));
    fgets(nuova_riga->Array, lungh_testo, stdin);
    current_command->nuova_prima = nuova_riga;
    if (rig1 > count){
        /* inserimento nuovo: o in testa o coda */
        if (count == 0){
            /* lista vuota: inserisco la nuova prima frase, in testa */
            first_line = nuova_riga;
            nuova_riga->prec = NULL;
            current_command->vecchia_ultima = NULL;
        }
        else{
            /* lista non vuota: attacco alla coda le nuove frasi */
            new_command->vecchia_ultima = last_line;
            nuova_riga->prec = last_line;
            last_line->next = nuova_riga;
        }
        current_command->vecchia_prima = NULL;
        
        for (int i = 1; i < nrig; ++i){
            /* creo le nuove righe */
            nuova_riga->next = malloc(sizeof(t_node));
            nuova_riga->next->prec = nuova_riga;
            nuova_riga = nuova_riga->next;
            fgets(nuova_riga->Array, lungh_testo, stdin);
        }
        nuova_riga->next = NULL;
        last_line = nuova_riga;
        current_command->nuova_ultima = nuova_riga;
        current_command->mod_count = nrig;
    }
    else{
        /* sostituzione di frasi */
        /* cerco la prima riga da sostituire */
        long int indiceRiga = 1;
        t_node *riga_da_cambiare = first_line;
        while ((riga_da_cambiare != NULL) && (indiceRiga != rig1)){
            indiceRiga++;
            riga_da_cambiare = riga_da_cambiare->next;
        }
        current_command->vecchia_prima = riga_da_cambiare;
        nuova_riga->prec = riga_da_cambiare->prec;
        if (riga_da_cambiare->prec == NULL){
            /* è la testa */
            first_line = nuova_riga;
        }
        else{
            /* sostituisco in mezzo */
            riga_da_cambiare->prec->next = nuova_riga;
        }
        long int Passi = 1;
        for (long int i = 1; i < nrig; ++i)
        { 
            /* leggo le rimanenti righe */
            nuova_riga->next = malloc(sizeof(t_node));
            nuova_riga->next->prec = nuova_riga;
            nuova_riga = nuova_riga->next;
            fgets(nuova_riga->Array, lungh_testo, stdin);
            if ((riga_da_cambiare != NULL) && (riga_da_cambiare->next != NULL)){
                riga_da_cambiare = riga_da_cambiare->next;
                Passi++;
            }
        }
        nuova_riga->next = riga_da_cambiare->next;
        current_command->nuova_ultima = nuova_riga;
        current_command->vecchia_ultima = riga_da_cambiare;
        if (riga_da_cambiare->next == NULL){
            /* sostituisco anche la coda quindi la aggiorno */
            last_line = nuova_riga;
            nrig -= Passi;  //tolgo il numero di righe che ho sostituito e non aggiunto al conteggio 
            current_command->mod_count = nrig;
        }
        else{
            /* sostituisco in mezzo quindi riaggancio alla lista */
            nuova_riga->next->prec = nuova_riga;
        }
    }
    if (rig2 > count){
        count = rig2;
    }
}

void Print(char *istr)
{
    long int rig1 = strtoimax(strtok(istr, ","), NULL, 10);
    long int rig2 = strtoimax(strtok(NULL, "p"), NULL, 10);
    long int nrig = rig2 - rig1 + 1;

    t_node *scorr;
    scorr = first_line;

    if (rig1 == 0)
    {
        if (rig1 < rig2){
            rig1++;
        }
        else{
            fputs(".\n", stdout);
            nrig--;
        }
    }
    for (int i = 1; ((i < rig1)&&(scorr != NULL)); ++i) {
        /* trovo ind1 */
        scorr = scorr->next;
    }
    for (int i = 1; i <= nrig; i++){
        if (scorr != NULL){
            /* ci sono frasi da stampare */
            fputs(scorr->Array, stdout);
            scorr = scorr->next;
        }
        else{
            /* oltre stampo punti */
            fputs(".\n", stdout);
        }
    }
}

void Delate(char *istr)
{
    long int rig1 = strtoimax(strtok(istr, ","), NULL, 10);
    long int rig2 = strtoimax(strtok(NULL, "d"), NULL, 10);
    long int nrig = rig2 - rig1 + 1;

    t_command *new_command = malloc(sizeof(t_command));
    new_command->istruzione = 'd';
    new_command->ind1 = rig1;
    new_command->ind2 = rig2;
    new_command->mod_count = 0;
    new_command->next = NULL;
    new_command->prec = newest_command;
    if (newest_command != NULL)
    {
        new_command->prec->next = new_command;
    }
    else{
        /* primo comando */
        oldest_command = new_command;
    }
    newest_command = new_command;
    current_command = new_command;
    current_command->nuova_prima = NULL;
    current_command->nuova_ultima = NULL;

    if (rig1 > count)
    {
        /* delate nulla */
        current_command->istruzione = 'n'; //così la riconosco subito e non fa nulla in undo o redo
        nrig = 0;
    }
    else if (rig1 == 0){
        if (rig1 < rig2){
            /* ad esempio 0,Xd */
            rig1++;
            nrig--;
        }
        else{
            /* allora 0,0d */
            current_command->istruzione = 'n';
            nrig = 0;
        }
    }
    else if ((nrig >= count) && (rig1 == 1))
    {
        /* cancello TUTTA la lista */
        current_command->vecchia_prima = first_line;
        current_command->vecchia_ultima = last_line;
        current_command->mod_count = count;
        first_line = current_command->nuova_prima;
        last_line = current_command->nuova_ultima;
    }
    else
    {
        long int indiceRiga = 1;
        t_node *riga_da_eliminare = first_line;
        while ((riga_da_eliminare != NULL) && (indiceRiga != rig1)){
            indiceRiga++;
            riga_da_eliminare = riga_da_eliminare->next;
        }
        current_command->vecchia_prima = riga_da_eliminare;
        t_node *riga_precedente = riga_da_eliminare->prec;
        long int Passi = 1;  //conto quante ne sto eliminando effettivamente 
        for (long int i = rig1; ((i < rig2) && (riga_da_eliminare->next != NULL)); ++i){
            riga_da_eliminare = riga_da_eliminare->next;
            Passi++;
        }
        current_command->vecchia_ultima = riga_da_eliminare;
        if (riga_precedente == NULL){
            /* sto eliminando la testa */
            current_command->nuova_prima = riga_da_eliminare->next;
            first_line = riga_da_eliminare->next;
            first_line->prec = NULL;
        }
        else if (riga_da_eliminare->next == NULL){
            /* ho eliminato anche la coda */
            current_command->nuova_ultima = riga_precedente;
            last_line = riga_precedente;
            last_line->next = NULL;
        }
        else if ((riga_precedente != NULL)&&(riga_da_eliminare->next != NULL)){
            /* ho eliminato frasi in mezzo */
            current_command->nuova_prima = riga_precedente;
            current_command->nuova_ultima = riga_da_eliminare->next;
            riga_precedente->next = riga_da_eliminare->next;
            riga_da_eliminare->next->prec = riga_precedente;
        }
        current_command->mod_count = Passi;  //mi salvo il numero effettivo di righe eliminate 
    }
    /* aggiorno numero righe */
    count -= new_command->mod_count;
}

void Undo(long int undo)
{
    if (current_command == NULL){ /* ho fatto redo dell'ultima istruzione disponibile */
        current_command = newest_command;
    }
    while (undo > 0)
    {
        if (current_command->istruzione == 'c')
        {
            if (current_command->vecchia_prima == NULL)
            {   /* caso di una aggiunta in coda */

                /* caso aggiunta in testa */
                if (current_command->ind1 == 1){
                    first_line = current_command->vecchia_prima;
                }
                last_line = current_command->vecchia_ultima;
                /* caso aggiunta in coda */
                if (current_command->vecchia_ultima != NULL){
                    last_line->next = NULL;
                }
            }
            else
            {   /* caso di sostituzione */
                current_command->vecchia_ultima->next = current_command->nuova_ultima->next;
                if (current_command->nuova_ultima->next != NULL){
                    /* ho sostituito non oltre ultima quindi riaggancio frase successiva*/
                    current_command->nuova_ultima->next->prec = current_command->vecchia_ultima;
                }
                else{
                    /* ho sostituito la coda */
                    last_line = current_command->vecchia_ultima;
                }

                current_command->vecchia_prima->prec = current_command->nuova_prima->prec;
                if (current_command->nuova_prima->prec != NULL){
                    /* non ho sostituito la testa */
                    current_command->nuova_prima->prec->next = current_command->vecchia_prima;
                }
                else{
                    first_line = current_command->vecchia_prima;
                }
            }
            /* se era un' aggiunta ora sto togliendo */
            /* se era una sostituzione senza aumentare, è inizializzato a zero quindi non sottrae nulla */
            count -= current_command->mod_count;
        }
        else if (current_command->istruzione == 'd')
        {
            if (current_command->nuova_ultima == NULL){
                /* avevo cancellato tutto o la testa */
                first_line = current_command->vecchia_prima;
                first_line->prec = NULL;
                if (current_command->nuova_prima != NULL){
                    /* la testa */
                    current_command->nuova_prima->prec = current_command->vecchia_ultima;
                    current_command->vecchia_ultima->next = current_command->nuova_prima;
                }
            }
            if (current_command->nuova_prima == NULL){
                /* avevo cancellato tutto o la coda */
                last_line = current_command->vecchia_ultima;
                last_line->next = NULL;
                if (current_command->nuova_ultima != NULL){
                    /* la coda */
                    current_command->nuova_ultima->next = current_command->vecchia_prima;
                    current_command->vecchia_prima->prec = current_command->nuova_ultima;
                }
            }
            else if ((current_command->nuova_prima != NULL)&&(current_command->nuova_ultima != NULL)){
                /* avevo eliminato in mezzo */
                current_command->nuova_prima->next = current_command->vecchia_prima;
                current_command->vecchia_prima->prec = current_command->nuova_prima;
                current_command->nuova_ultima->prec = current_command->vecchia_ultima;
                current_command->vecchia_ultima->next = current_command->nuova_ultima;
            }
            /* aggiorno il numero di righe */
            count += current_command->mod_count;

        }
        undo--;
        current_command = current_command->prec;
        if (current_command == NULL){
            /* ho fatto ultimo undo possibile */
            undo = 0;
        }
    }
}

void Redo(long int redo)
{
    if (current_command == NULL){ /* ho fatto undo dell'ultima istruzione disponibile */
        current_command = oldest_command;
    }
    else if (current_command->next != NULL){
        /* se ho fatto undo sono all'istruzione precedente quindi mi devo spostare avanti per tornare al blocco da ripristinare */
        current_command = current_command->next;
    }

    while (redo > 0)
    {
        if (current_command->istruzione == 'c')
        {
            if (current_command->vecchia_prima == NULL) {
                /* riaggiunta in coda */

                /* in testa */
                if (current_command->ind1 == 1) {
                    first_line = current_command->nuova_prima;
                    first_line->prec = NULL;
                }
                else{ /* in coda */
                    current_command->nuova_prima->prec = current_command->vecchia_ultima;
                    current_command->vecchia_ultima->next = current_command->nuova_prima;
                }
                last_line = current_command->nuova_ultima;
                last_line->next = NULL;
            }
            else{
                /* caso di risostituzione */

                current_command->nuova_ultima->next = current_command->vecchia_ultima->next;
                if (current_command->vecchia_ultima->next != NULL){
                    /* sostituisco non oltre ultima quindi riaggancio frase successiva */
                    current_command->vecchia_ultima->next->prec = current_command->nuova_ultima;
                }
                else{
                    /* ho sostituito la coda */
                    last_line = current_command->nuova_ultima;
                }

                current_command->nuova_prima->prec = current_command->vecchia_prima->prec;
                if (current_command->vecchia_prima->prec != NULL){
                    /* non ho sostituito la testa */
                    current_command->vecchia_prima->prec->next = current_command->nuova_prima;
                }
                else{
                    first_line = current_command->nuova_prima;
                }
            }
            count += current_command->mod_count;
        }

        if (current_command->istruzione == 'd')
        {
            if (current_command->nuova_ultima == NULL){
                /* ri cancello tutto o da testa */
                first_line = current_command->nuova_prima;
                if (current_command->nuova_prima != NULL){
                    current_command->nuova_prima->prec = NULL;
                }
            }
            if (current_command->nuova_prima == NULL){
                /* ri cancello tutto o da coda */
                last_line = current_command->nuova_ultima;
                if (current_command->nuova_ultima != NULL){
                    current_command->nuova_ultima->next = NULL;
                }
            }
            else if ((current_command->nuova_prima != NULL)&&(current_command->nuova_ultima != NULL)){
                /* ri cancello in mezzo */
                current_command->nuova_prima->next = current_command->nuova_ultima;
                current_command->nuova_ultima->prec = current_command->nuova_prima;
            }
            /* aggiorno numero di righe */
            count -= current_command->mod_count;
        }
        redo--;
        if (redo > 0){
            if (current_command->next != NULL){
                current_command = current_command->next;
            }
            else{
                newest_command = current_command;
                current_command = current_command->next;
                redo = 0;
            }
        }
    }
}

void SvuotaLista(){
    t_command *temp;
    temp = newest_command;
    while (temp != current_command){
        newest_command = newest_command->prec;
        free(temp);
        temp = newest_command;
    }
}
