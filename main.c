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
    short *terms_indices;
    char *term;
    short length;
} Implicant;
typedef struct
{
    Term *terms;
    short *gray_code;
    short *min_terms_indices;
    short *max_terms_indices;
    short min_terms_count;
    short max_terms_count;
} TruthTable;

short *gray_code();
char *generate_bits(short);
TruthTable set_truth_table(char *);
short check_output(TruthTable);
Implicant **group_terms(TruthTable, short *, short **, char);
Term set_term(char *, short, char);
Implicant set_implicant_from_tt(Term *, char *, short);
Implicant set_implicant(short *, char *, short);
Implicant *set_prime_implicants(Implicant **, Implicant **, short, short *, short *);
Implicant *get_essential_prime_implicants(Implicant *, TruthTable, short, short *, char);
void QuineMcClusky_method(TruthTable, char);
short compare_bits(char *, char *);
void set_n_bits();
void table_print(TruthTable);
void k_map_print(TruthTable);
short count_ones(char *);
void alphabit_term_print(char *, char);
void print_imp(Implicant imp);
static short g_n_bits = 0, g_n_combos = 0;

void main()
{
    TruthTable truth_table;
    char exp_type;
    set_n_bits();
    truth_table = set_truth_table(&exp_type);
    if (check_output(truth_table))
    {
        k_map_print(truth_table);
        if (exp_type == '1' || exp_type == '2')
            QuineMcClusky_method(truth_table, '1');
        if (exp_type == '0' || exp_type == '2')
            QuineMcClusky_method(truth_table, '0');
    }
}
/* Generates an array of gray code pattern in decimal form */
short *gray_code()
{
    short *code = (short *)calloc(g_n_combos, sizeof(short));
    for (int i = 0; i < g_n_combos; i++)
    {
        code[i] = i ^ (i >> 1);
    }
    return code;
}
/* Genereates a string representing the bits equivalent to certain decimal */
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
/* Sets the global variables of the number of bits and number of combinations according to the user's input */
void set_n_bits()
{
    printf("\t*Welcome to boolean function generator*\n\n");
    do
    {
        printf("Enter number of bits: \n");
        scanf("%d", &g_n_bits);
        if (g_n_bits <= 1)
            printf("Please enter a proper number of bits which is greater than 1.\n", g_n_bits);
    } while (g_n_bits <= 1);
    g_n_combos = powf(2, g_n_bits);
}
/* Sets the truth table and its outputs */
TruthTable set_truth_table(char *exp_type)
{
    TruthTable truth_table;
    Term *table_terms = (Term *)calloc(g_n_combos, sizeof(Term));
    char *bits;
    short output, *min_terms_indices, *max_terms_indices;

    do
    {
        printf("Enter the proper nubmer to choose the type of expression:\n0:POS 1:SOP 2:Both\n");
        scanf("%d", &output);
        switch (output)
        {
        case 0:
            *exp_type = '0';
            break;
        case 1:
            *exp_type = '1';
            break;
        case 2:
            *exp_type = '2';
            break;
        default:
            break;
        }
    } while (output < 0 && output > 2);

    if (*exp_type == '1' || *exp_type == '2')
        min_terms_indices = (short *)malloc(sizeof(short));
    if (*exp_type == '0' || *exp_type == '2')
        max_terms_indices = (short *)malloc(sizeof(short));

    do
    {
        int min_terms_count = 0, max_terms_count = 0;
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
            if (output && (*exp_type == '1' || *exp_type == '2'))
            {
                min_terms_indices = (short *)realloc(min_terms_indices, sizeof(short) * ++min_terms_count);
                min_terms_indices[min_terms_count - 1] = i;
            }
            else if (!output && (*exp_type == '0' || *exp_type == '2'))
            {
                max_terms_indices = (short *)realloc(max_terms_indices, sizeof(short) * ++max_terms_count);
                max_terms_indices[max_terms_count - 1] = i;
            }
        }
        truth_table = (TruthTable){table_terms, gray_code(), min_terms_indices, max_terms_indices, min_terms_count, max_terms_count};
        table_print(truth_table);
        printf("Proceed with this table? (enter the proper number)\n1:Yes 0:No\n");
        scanf("%d", &output);
    } while (output == 0);
    return truth_table;
}
/* Check that the outputs enetered by the user are not all ones or zeros */
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
/* Prints truth table */
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
/* Prints K-map */
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
/* Initalizes a term with its bits, order and output */
Term set_term(char *term, short index, char output)
{
    Term main_term = {(char *)malloc(g_n_bits + 1), index, output};
    strcpy(main_term.term, term);
    return main_term;
}
/* Returns the index of the one bit diff or -2 if both matches or -1 if totally different */
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
/* Sets the the matched bit to '_' */
char *set_matched_bit(char *str, short index)
{
    str[index] = '_';
    return str;
}
/* Sets the implicant */
Implicant set_implicant_from_tt(Term *terms, char *term, short length)
{
    short *indices = (short *)calloc(length, sizeof(short));
    for (short i = 0; i < length; i++)
        indices[i] = terms[i].index;
    return (Implicant){indices, term, length};
}
/* Sets the implicant */
Implicant set_implicant(short *terms_indices, char *term, short length)
{
    short *indices = (short *)calloc(length, sizeof(short));
    for (short i = 0; i < length; i++)
        indices[i] = terms_indices[i];
    return (Implicant){indices, term, length};
}
/* Group the terms into groups of implicants according to number of ones */
Implicant **group_terms(TruthTable truth_table, short *n_groups, short **n_imps_in_group, char exp_type)
{
    Implicant **groups = (Implicant **)malloc(sizeof(Implicant *));
    *n_imps_in_group = (short *)malloc(sizeof(short));
    short k = 0, n_ones, last_n_ones = -1, group_i, count, *indices;
    *n_groups = 0;
    if (exp_type == '1')
    {
        count = truth_table.min_terms_count;
        indices = truth_table.min_terms_indices;
    }
    else if (exp_type == '0')
    {
        count = truth_table.max_terms_count;
        indices = truth_table.max_terms_indices;
    }

    for (short i = 0; i < count; i++)
    {
        n_ones = count_ones((truth_table.terms[indices[i]]).term);
        if (last_n_ones < n_ones)
        {
            groups = (Implicant **)realloc(groups, sizeof(Implicant *) * ++*n_groups);
            *n_imps_in_group = (short *)realloc(*n_imps_in_group, sizeof(short) * *n_groups);
            n_imps_in_group[0][*n_groups - 1] = 0;
            groups[*n_groups - 1] = (Implicant *)malloc(sizeof(Implicant));
            last_n_ones = n_ones;
        }
        group_i = *n_groups - 1 - last_n_ones + n_ones;
        groups[group_i] = (Implicant *)realloc(groups[group_i], sizeof(Implicant) * ++n_imps_in_group[0][group_i]);
        groups[group_i][n_imps_in_group[0][group_i] - 1] = set_implicant_from_tt(&truth_table.terms[indices[i]], truth_table.terms[indices[i]].term, 1);
    }
    return groups;
}
/* Count the ones in a term */
short count_ones(char *bits)
{
    short n_ones = 0;
    for (short i = 0; i < g_n_bits; i++)
        if (*(bits + i) == '1')
            n_ones++;
    return n_ones;
}
/* Recursively group prime implicants */
Implicant *set_prime_implicants(Implicant **last_level, Implicant **prime_implicants, short n_group, short *n_imps_in_g, short *prime_count)
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
        for (short j = 0; j < n_imps_in_g[i]; j++)
        {
            is_bit_diff = 0;
            // loops over the next imps of the next group
            for (short k = 0, x = 1; k < n_imps_in_g[q]; k++, x = 1)
            {
                comparison = compare_bits(last_level[i][j].term, last_level[q][k].term);
                if (comparison >= 0 && i < n_group - 1)
                {
                    checked_imps = (short **)realloc(checked_imps, sizeof(short *) * (matched_count += 2));
                    checked_imps[matched_count - 2] = last_level[i][j].terms_indices;
                    checked_imps[matched_count - 1] = last_level[q][k].terms_indices;
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
                        new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].terms_indices = (short *)malloc(sizeof(short));

                        for (short x = 0; x < 2; x++)
                        {
                            for (short y = 0; y < last_level[i + x][(x) ? k : j].length; y++)
                            {
                                new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].terms_indices = (short *)realloc(new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].terms_indices, sizeof(short) * ++(new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].length));
                                new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].terms_indices[new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].length - 1] = last_level[i + x][(x) ? k : j].terms_indices[y];
                            }
                        }
                        new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].term = malloc(g_n_bits);
                        new_level[groups_count - 1][n_in_groups[groups_count - 1] - 1].term = new_term;
                    }
                    else
                        free(new_term);
                }
            }
            if (!is_bit_diff)
            {
                for (short m = 0; m < matched_count; m++)
                {
                    if (checked_imps[m] == last_level[i][j].terms_indices)
                        is_bit_diff = 1;
                }
                if (!is_bit_diff)
                {
                    prime_implicants[0] = (Implicant *)realloc(prime_implicants[0], sizeof(Implicant) * ++*prime_count);
                    prime_implicants[0][*prime_count - 1] = set_implicant(last_level[i][j].terms_indices, last_level[i][j].term, last_level[i][j].length);
                }
            }
        }
    }
    if (groups_count != 0)
        set_prime_implicants(new_level, prime_implicants, groups_count, n_in_groups, prime_count);
    return *prime_implicants;
}
/* Gets the essential prime implicants from the prime implicants */
Implicant *get_essential_prime_implicants(Implicant *prime_imps, TruthTable truth_table, short prime_count, short *es_prime_count, char exp_type)
{
    Implicant *es_prime_implicants = (Implicant *)malloc(sizeof(Implicant));
    short *indices, count;
    *es_prime_count = 0;
    if (exp_type == '1')
    {
        indices = truth_table.min_terms_indices;
        count = truth_table.min_terms_count;
    }
    else if (exp_type == '0')
    {
        indices = truth_table.max_terms_indices;
        count = truth_table.max_terms_count;
    }
    short terms_occur[count];
    for (short i = 0; i < count; i++)
        terms_occur[i] = 0;

    for (short m = 0; m < count; m++)
        for (short i = 0; i < prime_count; i++)
            for (short j = 0; j < prime_imps[i].length; j++)
                if (indices[m] == prime_imps[i].terms_indices[j])
                    terms_occur[m]++;

    for (short k = 0; k < count; k++)
    {
        if (terms_occur[k] == 1)
        {
            for (short i = 0; i < prime_count; i++)
            {
                for (short j = 0, x = 0; j < prime_imps[i].length; j++, x = 0)
                {
                    if (prime_imps[i].terms_indices[j] == indices[k])
                    {
                        for (short n = 0; n < *es_prime_count; n++)
                            if (es_prime_implicants[n].term == prime_imps[i].term)
                                x = 1;
                        if (x == 0)
                        {
                            es_prime_implicants = (Implicant *)realloc(es_prime_implicants, sizeof(Implicant) * ++*es_prime_count);
                            es_prime_implicants[*es_prime_count - 1] = set_implicant(prime_imps[i].terms_indices, prime_imps[i].term, prime_imps[i].length);
                            break;
                        }
                    }
                }
            }
        }
    }
    return es_prime_implicants;
}
/* Executes the Quine-McCluskey Algorithm on a truth table */
void QuineMcClusky_method(TruthTable truth_table, char exp_type)
{
    short n_groups = 0, *n_mins_in_g, prime_count = 0, es_prime_count = 0;
    Implicant **grouped_terms, *prime_imps, *es_prime_imps;
    prime_imps = (Implicant *)malloc(sizeof(Implicant));
    grouped_terms = group_terms(truth_table, &n_groups, &n_mins_in_g, exp_type);
    prime_imps = set_prime_implicants(grouped_terms, &prime_imps, n_groups, n_mins_in_g, &prime_count);
    es_prime_imps = get_essential_prime_implicants(prime_imps, truth_table, prime_count, &es_prime_count, exp_type);
    printf("\n");
    if (exp_type == '1')
        printf("%s", "The SOP expression: ");
    else if (exp_type == '0')
        printf("%s", "The POS expression: ");

    for (short i = 0; i < es_prime_count; i++)
    {
        alphabit_term_print(es_prime_imps[i].term, exp_type);
        if (exp_type == '1' && (i != es_prime_count - 1))
            printf("%c", '+');
    }
    printf("\n");
}
/* Prints the content of an Implicant */
void print_imp(Implicant imp)
{
    printf("Implicant:\n%s\n", imp.term);
    for (short i = 0; i < imp.length; i++)
        printf("%d ", imp.terms_indices[i]);

    printf("\n\n");
}
/* Prints the alphabit representation of bits */
void alphabit_term_print(char *term, char exp_type)
{
    if (exp_type == '1')
    {
        for (short i = 0; i < g_n_bits; i++)
            if (term[i] == '1')
                printf("%c", 'A' + i);
            else if (term[i] == '0')
                printf("%c`", 'A' + i);
    }
    else if (exp_type == '0')
    {
        printf("(");
        for (short i = 0, plus = 0; i < g_n_bits; i++, plus = 0)
        {
            if (term[i] == '0')
            {
                printf("%c%c", 'A' + i);
            }
            else if (term[i] == '1')
            {
                printf("%c`%c", 'A' + i);
            }
            if (term[i] != '_')
            {
                for (short j = i + 1; j < g_n_bits; j++)
                {
                    if (term[j] != '_')
                        plus = 1;
                }
                if (plus == 1)
                    printf("+");
            }
        }
        printf(")");
    }
}