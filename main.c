#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#pragma pack(1)

typedef struct
{
    char *term;
    short index;
    char output;
} Term;
typedef struct
{
    short *min_terms_indices;
    char *term;
    short length;
} Implicant;
typedef struct
{
    Term *terms;
    short *gray_code;
    short *min_terms_indices;
    short min_terms_count;
} TruthTable;

short *gray_code();           // tick
char *generate_bits(short);   // tick
TruthTable set_truth_table(); // tick
short check_output(TruthTable);
Implicant **group_min_terms(TruthTable, short *, short **); // tick
Term set_term(char *, short, char);                         // tick
Implicant set_implicant_from_tt(Term *, char *, short);     // tick
Implicant set_implicant(short *, char *, short);
Implicant *set_prime_implicants(Implicant **, Implicant **, short, short *, short *);
Implicant *get_essential_prime_implicants(Implicant *, TruthTable, short, short *);
void QuineMcClusky_method(TruthTable);
short compare_bits(char *, char *); // tick
void set_n_bits();                  // tick
void table_print(TruthTable);       // tick
void k_map_print(TruthTable);       // tick
short count_ones(char *);           // tick
void alphabit_term_print(char *);
void print_imp(Implicant imp);
static short g_n_bits = 0, g_n_combos = 0;

void main()
{
    TruthTable truth_table;
    set_n_bits();
    truth_table = set_truth_table();
    if (check_output(truth_table))
    {
        k_map_print(truth_table);
        QuineMcClusky_method(truth_table);
    }
}

short *gray_code()
{
    short *code = (short *)calloc(g_n_combos, sizeof(short));
    for (int i = 0; i < g_n_combos; i++)
    {
        code[i] = i ^ (i >> 1);
    }
    return code;
}
char *generate_bits(short decimal)
{
    char *bits = (char *)calloc(g_n_bits, 1);
    for (short i = g_n_bits - 1; i >= 0; i--)
    {
        bits[i] = (decimal % 2) ? '1' : '0';
        decimal /= 2;
    }
    return bits;
}
void set_n_bits()
{
    printf("\t*Welcome to boolean function generator*\n\n");
    do
    {
        printf("Enter number of bits: \n");
        scanf("%d", &g_n_bits);
        if (g_n_bits <= 1)
            printf("Dude are you trolling am I gonna make an equation for %d bits? Please enter a proper number of bits.\n", g_n_bits);
    } while (g_n_bits <= 1);
    g_n_combos = powf(2, g_n_bits);
}
/* Sets the truth table and its outputs */
TruthTable set_truth_table()
{
    TruthTable truth_table;
    Term *table_terms = (Term *)calloc(g_n_combos, sizeof(Term));
    char *bits;
    short output, *min_term_indices = (short *)malloc(sizeof(short));
    do
    {
        int min_terms_count = 0;
        printf("Enter the outputs of the truth table:\n");
        for (short i = 0; i < g_n_bits; i++)
            printf("%c", 'A' + i);

        printf(": O/P\n");
        for (short i = 0; i < g_n_combos; i++)
        {
            bits = generate_bits(i);
            printf("%s: ", bits);
            scanf("%d", &output);
            table_terms[i] = set_term(bits, i, (output) ? '1' : '0');
            if (output)
            {
                min_term_indices = (short *)realloc(min_term_indices, sizeof(short) * (min_terms_count + 1));
                min_term_indices[min_terms_count] = i;
                min_terms_count += 1;
            }
        }
        truth_table = (TruthTable){table_terms, gray_code(), min_term_indices, (short)min_terms_count};
        table_print(truth_table);
        printf("Are you okay with the previous table?\nEnter 1=>for yes || 0=>for no\n");
        scanf("%d", &output);
    } while (!output);
    return truth_table;
}
short check_output(TruthTable truth_table)
{
    short ones = 1, zeros = 0;
    for (short i = 0; i < g_n_combos; i++)
    {
        if (truth_table.terms[i].output == '1')
            zeros = 1;
        else
            ones = 0;
    }
    if (ones == 1 && zeros == 1)
    {
        printf("The output is 1, no equation needed!\n");
        return 0;
    }
    else if (ones == 0 && zeros == 0)
    {
        printf("The output is 0, no equation needed!\n");
        return 0;
    }
    else
        return 1;
}
// Prints truth table
void table_print(TruthTable truth_table)
{
    printf("\n");
    for (short i = 0; i < g_n_bits; i++)
        printf("%c", 'A' + i);
    printf(" | O/P\n");
    for (short i = 0; i < g_n_bits + 6; i++)
        printf("-");
    printf("\n");
    for (short i = 0; i < g_n_combos; i++)
        printf("%s | %c\n", (truth_table.terms[i]).term, (truth_table.terms[i]).output);
}
// Prints K-map
void k_map_print(TruthTable truth_table)
{

    short after_n_bits = (g_n_bits % 2 == 0) ? (g_n_bits / 2) : (g_n_bits / 2 + 1), revert = 0, k = 0, l = 0;
    short rows = powf(2, after_n_bits), columns = powf(2, g_n_bits - after_n_bits);
    printf("\n");
    for (short i = 0; i < g_n_bits; i++)
    {
        printf("%c", 'A' + i);
        if (i == after_n_bits - 1)
            printf("\\");
    }
    printf("\n");
    for (short i = 0; i < after_n_bits + 2; i++)
        printf(" ");
    for (short i = 0; i < columns; i++)
    {
        for (short j = after_n_bits; j < g_n_bits; j++)
            printf("%c", truth_table.terms[truth_table.gray_code[i]].term[j]);
        printf(" ");
    }
    printf("\n");
    for (short i = 0; i < after_n_bits + 1; i++)
        printf(" ");
    for (short i = 0; i < columns * (g_n_bits - after_n_bits + 1); i++)
        printf("-");
    printf("\n");
    for (short i = 0; i < rows; i++)
    {
        for (short j = (g_n_bits % 2 == 0) ? after_n_bits : after_n_bits - 1; j < g_n_bits + 1; j++)
            printf("%c", truth_table.terms[truth_table.gray_code[i]].term[j]);
        printf(" |");
        for (; l < columns * (i + 1); l++)
        {
            printf("%c", truth_table.terms[truth_table.gray_code[(revert) ? k-- : k++]].output);
            for (short i = 0; i < g_n_bits - after_n_bits; i++)
            {
                printf(" ");
            }
        }
        k += (!revert) ? columns - 1 : columns + 1;
        revert = !revert;
        printf("\n");
    }
}
// Initalizes a term with its bits, order and output
Term set_term(char *term, short index, char output)
{
    Term main_term = {(char *)malloc(g_n_bits + 1), index, output};
    strcpy(main_term.term, term);
    return main_term;
}
// Returns the index of the one bit diff or -2 if both matches or -1 if totally different
short compare_bits(char *str1, char *str2)
{
    short bit_diff_index, diffs = 0;
    for (short i = 0; i < g_n_bits; i++)
    {
        if (str1[i] != str2[i])
        {
            diffs++;
            bit_diff_index = i;
        }
    }
    if (diffs == 1)
        return bit_diff_index;
    else if (diffs == 0)
        return -2;
    else
        return -1;
}
// Sets the the matched bit to '-'
char *set_matched_bit(char *str, short index)
{
    str[index] = '_';
    return str;
}
// Sets the implicant
Implicant set_implicant_from_tt(Term *terms, char *term, short length)
{
    short *min_terms_indices = (short *)calloc(length, sizeof(short));
    for (short i = 0; i < length; i++)
        min_terms_indices[i] = (terms[i]).index;
    return (Implicant){min_terms_indices, term, length};
}
// Sets the implicant
Implicant set_implicant(short *min_terms, char *term, short length)
{
    short *min_terms_indices = (short *)calloc(length, sizeof(short));
    for (short i = 0; i < length; i++)
        min_terms_indices[i] = min_terms[i];
    return (Implicant){min_terms_indices, term, length};
}
// Group the min terms into groups of implicants according to number of ones
Implicant **group_min_terms(TruthTable truth_table, short *n_groups, short **n_implicants_in_group)
{
    Implicant **min_groups = (Implicant **)malloc(sizeof(Implicant *));
    *n_implicants_in_group = (short *)malloc(sizeof(short));
    short k = 0, n_ones, last_n_ones = -1, group_i;
    *n_groups = 0;
    for (short i = 0; i < truth_table.min_terms_count; i++)
    {
        n_ones = count_ones((truth_table.terms[truth_table.min_terms_indices[i]]).term);
        if (last_n_ones < n_ones)
        {
            min_groups = (Implicant **)realloc(min_groups, sizeof(Implicant *) * ++*n_groups);
            *n_implicants_in_group = (short *)realloc(*n_implicants_in_group, sizeof(short) * *n_groups);
            n_implicants_in_group[0][*n_groups - 1] = 0;
            min_groups[*n_groups - 1] = (Implicant *)malloc(sizeof(Implicant));
            last_n_ones = n_ones;
        }
        group_i = *n_groups - 1 - last_n_ones + n_ones;
        min_groups[group_i] = (Implicant *)realloc(min_groups[group_i], sizeof(Implicant) * ++n_implicants_in_group[0][group_i]);
        min_groups[group_i][n_implicants_in_group[0][group_i] - 1] = set_implicant_from_tt(&truth_table.terms[truth_table.min_terms_indices[i]], truth_table.terms[truth_table.min_terms_indices[i]].term, 1);
    }
    return min_groups;
}
// Count the ones in a term
short count_ones(char *bits)
{
    short n_ones = 0;
    for (short i = 0; i < g_n_bits; i++)
        if (*(bits + i) == '1')
            n_ones++;
    return n_ones;
}
//
Implicant *set_prime_implicants(Implicant **last_level, Implicant **prime_implicants, short n_group, short *n_implicants_in_g, short *prime_count)
{
    Implicant **new_level = (Implicant **)malloc(sizeof(Implicant *));
    short is_bit_diff, comparison, groups_count = 0, last = -1, *n_in_groups = (short *)malloc(sizeof(short)), matched_count = 0;
    short **checked_imps = (short **)malloc(sizeof(char *));
    char *new_term;
    // loops over groups of implicants
    for (short i = 0, q = i + 1; i < n_group; i++, q = i + 1)
    {
        if (i == n_group - 1)
        {
            q = i - 1;
        }
        // loops over imps of each group
        for (short j = 0; j < n_implicants_in_g[i]; j++)
        {
            is_bit_diff = 0;
            // loops over the next imps of the next group
            for (short k = 0, x = 1; k < n_implicants_in_g[q]; k++, x = 1)
            {
                comparison = compare_bits(last_level[i][j].term, last_level[q][k].term);
                if (comparison >= 0 && i < n_group - 1)
                {
                    checked_imps = (short **)realloc(checked_imps, sizeof(short *) * (matched_count += 2));
                    checked_imps[matched_count - 2] = last_level[i][j].min_terms_indices;
                    checked_imps[matched_count - 1] = last_level[q][k].min_terms_indices;
                    new_term = (char *)malloc(g_n_bits);
                    new_term = set_matched_bit(strcpy(new_term, last_level[i][j].term), comparison);
                    for (short m = 0; m < n_in_groups[groups_count - 1]; m++)
                    {
                        if (strcmp(new_level[groups_count - 1][m].term, new_term) == 0)
                            x = 0;
                    }
                    if (x == 1)
                    {
                        if (last < i)
                        {
                            new_level = (Implicant **)realloc(new_level, sizeof(Implicant *) * ++groups_count);
                            new_level[groups_count - 1] = (Implicant *)malloc(sizeof(Implicant));
                            n_in_groups = (short *)realloc(n_in_groups, sizeof(short) * groups_count);
                            n_in_groups[groups_count - 1] = 0;
                            last = i;
                        }
                        is_bit_diff = 1;
                        new_level[groups_count - 1] = (Implicant *)realloc(new_level[groups_count - 1], sizeof(Implicant) * ++n_in_groups[groups_count - 1]);
                        new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].length = 0;
                        new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].min_terms_indices = (short *)malloc(sizeof(short));

                        for (short x = 0; x < 2; x++)
                        {
                            for (short y = 0; y < last_level[i + x][(x) ? k : j].length; y++)
                            {
                                new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].min_terms_indices = (short *)realloc(new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].min_terms_indices, sizeof(short) * ++(new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].length));
                                new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].min_terms_indices[new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].length - 1] = last_level[i + x][(x) ? k : j].min_terms_indices[y];
                            }
                        }
                        new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].term = malloc(g_n_bits);
                        new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].term = new_term;
                        print_imp(new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1]);
                    }
                    else
                        free(new_term);
                }
            }
            if (!is_bit_diff)
            {
                for (short m = 0; m < matched_count; m++)
                {
                    if (checked_imps[m] == last_level[i][j].min_terms_indices)
                        is_bit_diff = 1;
                }
                if (!is_bit_diff)
                {
                    prime_implicants[0] = (Implicant *)realloc(prime_implicants[0], sizeof(Implicant) * ++*prime_count);
                    prime_implicants[0][*prime_count - 1] = set_implicant(last_level[i][j].min_terms_indices, last_level[i][j].term, last_level[i][j].length);
                    printf("This is a prime Implicant:______\n");
                    print_imp(prime_implicants[0][*prime_count - 1]);
                    printf("\n");
                }
            }
        }
    }
    if (groups_count != 0)
        set_prime_implicants(new_level, prime_implicants, groups_count, n_in_groups, prime_count);
    return *prime_implicants;
}
Implicant *get_essential_prime_implicants(Implicant *prime_imps, TruthTable truth_table, short prime_count, short *es_prime_count)
{
    Implicant *es_prime_implicants = (Implicant *)malloc(sizeof(Implicant));
    *es_prime_count = 0;
    short min_term_occur[truth_table.min_terms_count];
    for (short i = 0; i < truth_table.min_terms_count; i++)
        min_term_occur[i] = 0;

    for (short m = 0; m < truth_table.min_terms_count; m++)
        for (short i = 0; i < prime_count; i++)
            for (short j = 0; j < prime_imps[i].length; j++)
                if (truth_table.min_terms_indices[m] == prime_imps[i].min_terms_indices[j])
                    min_term_occur[m]++;

    for (short k = 0; k < truth_table.min_terms_count; k++)
    {
        if (min_term_occur[k] == 1)
        {
            for (short i = 0; i < prime_count; i++)
            {
                for (short j = 0, x = 0; j < prime_imps[i].length; j++, x = 0)
                {
                    if (prime_imps[i].min_terms_indices[j] == truth_table.min_terms_indices[k])
                    {
                        for (short n = 0; n < *es_prime_count; n++)
                            if (es_prime_implicants[n].term == prime_imps[i].term)
                                x = 1;
                        if (x == 0)
                        {
                            es_prime_implicants = (Implicant *)realloc(es_prime_implicants, sizeof(Implicant) * ++*es_prime_count);
                            es_prime_implicants[*es_prime_count - 1] = set_implicant(prime_imps[i].min_terms_indices, prime_imps[i].term, prime_imps[i].length);
                            break;
                        }
                    }
                }
            }
        }
    }
    return es_prime_implicants;
}
void QuineMcClusky_method(TruthTable truth_table)
{
    short n_groups = 0, *n_mins_in_g, prime_count = 0, es_prime_count = 0;
    Implicant **grouped_min_terms, *prime_imps, *es_prime_imps;
    prime_imps = (Implicant *)malloc(sizeof(Implicant));
    grouped_min_terms = group_min_terms(truth_table, &n_groups, &n_mins_in_g);
    prime_imps = set_prime_implicants(grouped_min_terms, &prime_imps, n_groups, n_mins_in_g, &prime_count);
    es_prime_imps = get_essential_prime_implicants(prime_imps, truth_table, prime_count, &es_prime_count);
    printf("\n");
    for (short i = 0; i < es_prime_count; i++)
    {
        alphabit_term_print(es_prime_imps[i].term);
        printf("%c", (i != es_prime_count - 1) ? '+' : '\n');
    }
}
void print_imp(Implicant imp)
{
    printf("Implicant:\n%s\n", imp.term);
    for (short i = 0; i < imp.length; i++)
        printf("%d ", imp.min_terms_indices[i]);

    printf("\n\n");
}
void alphabit_term_print(char *term)
{
    for (short i = 0; i < g_n_bits; i++)
        if (term[i] == '1')
            printf("%c", 'A' + i);
        else if (term[i] == '0')
            printf("%c`", 'A' + i);
}