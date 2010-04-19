/*!
 * \file mei_hash_table.c
 *
 * \brief Hash table, intended to provide a symbol table
 */

/*
  This file is part of the "Mathematical Expression Interpreter" library.

  Copyright (C) 2008-2009  EDF

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
 * Standard C library headers
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>

/*----------------------------------------------------------------------------
 * BFT library headers
 *----------------------------------------------------------------------------*/

#include <bft_mem.h>
#include <bft_error.h>

/*----------------------------------------------------------------------------
 * Local headers
 *----------------------------------------------------------------------------*/

#include "mei_math_util.h"

/*----------------------------------------------------------------------------
 * Header for the current file
 *----------------------------------------------------------------------------*/

#include "mei_hash_table.h"

/*============================================================================
 * Private function definitions
 *============================================================================*/

/*!
 * \brief Hash function.
 *
 * \param [in] s key
 * \param [in] modulo interne parameter for hash table
 *
 * \return value associated to the key s
 */

static unsigned
_hash(const char *const s, const int modulo)
{
  unsigned h = 0;
  int i;

  for (i = 0; s[i] != '\0'; i++) {
    h = h*256 + (unsigned) s[i];
    if (h >= (unsigned) modulo)
      h %= modulo;
  }
  return h;
}

/*----------------------------------------------------------------------------
 *  Allocation d'une struct item
 *----------------------------------------------------------------------------*/

static struct item*
_new_item(const char *const key)
{
  struct item* new;

  // Allocation de l'item proprement dit
  BFT_MALLOC(new, 1, struct item);
  if (new == NULL)
    bft_error(__FILE__, __LINE__, 0,
              "Error in memory allocation\n");

  // Allocation de la mémoire pour stocker la chaîne
  BFT_MALLOC(new->key, strlen(key)+1, char);
  if (new->key == NULL)
    bft_error(__FILE__, __LINE__, 0,
              "Error in memory allocation\n");

  BFT_MALLOC(new->data, 1, data_t);
  if (new->data == NULL)
    bft_error(__FILE__, __LINE__, 0,
              "Error in memory allocation\n");

  return new;
}


/*============================================================================
 *  Fonctions publiques
 *============================================================================*/


/*----------------------------------------------------------------------------
 *  Fonction qui alloue une table de hachage
 *----------------------------------------------------------------------------*/


/* Initialize the hash table to the size (modulo) asked for.
 * Allocates space for the correct number of pointers and sets them to NULL.
 * If it can't allocate sufficient memory, signals error by setting the size
 * of the table to 0.
 */

void
mei_hash_table_create(hash_table_t *const htable, const int modulo)
{
  int i;

  /* htable length and record number */
  htable->n_inter = 0;
  htable->length  = modulo;
  htable->record  = 0;
  htable->table   = NULL;

  /* htable memory allocation */
  BFT_MALLOC(htable->table, modulo, struct item*);
  if (htable->table == NULL) {
    htable->length = 0;
    bft_error(__FILE__, __LINE__, 0,
              "Error in memory allocation\n");
  }

  /*htable default initialization: at the beginning all lists are empty */
  for (i = 0; i < modulo; i++)
    htable->table[i] = NULL;
}


/*----------------------------------------------------------------------------
 *  Fonction qui insert un element dans la table de hachage
 *----------------------------------------------------------------------------*/


// Recherche et insertion d'une chaîne dans une table
// Recherche une chaîne dans une table.  Si la chaîne
// est déjà présente dans la table, cette fonction
// retourne le pointeur qui la contient.  Sinon, la
// chaîne est mise dans la table et la fonction retourne
// le pointeur sur la cellule qui a été crée.
// Dans les deux cas, la fonction retourne un pointeur
// sur la cellule qui contient la chaîne.
struct item*
mei_hash_table_find(hash_table_t *const htable, const char *const key)
{
  unsigned v;
  struct item* l;
  struct item* p;

  /* Liste où doit se trouver la chaîne */
  v = _hash(key, htable->length);
  l = htable->table[v];

  /* Parcours de la liste */
  for ( p = l; p != 0; p = p->next)
    if (strcmp(p->key, key) == 0) return p;

  return NULL;
}


/*----------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/


// Insertion d'un identificateur dans une table
void
mei_hash_table_insert(hash_table_t *const htable,
                      const char *const  key,
                      const mei_flag_t type,
                      const double value,
                      const func1_t f1,
                      const func2_t f2,
                      const func3_t f3,
                      const func4_t f4)
{
  unsigned v;
  struct item* item = mei_hash_table_find(htable, key);

  if (item == NULL) {

    /* La chaine n'a pas été trouvée. On crée un nouvel item pour l'insérer */
    item = _new_item(key);

    item->type = type;
    if (type == FUNC1) {
      item->data->func = f1;

    } else if (type == FUNC2) {
      item->data->f2 = f2;

    } else if (type == FUNC3) {
      bft_error(__FILE__, __LINE__, 0, "not implemented yet \n");

    } else if (type == FUNC4) {
      bft_error(__FILE__, __LINE__, 0, "not implemented yet \n");

    } else {
      item->data->value = value;
    }
    strcpy(item->key, key);

    htable->record++;

    /* Insertion d'une cellule dans une liste */
    /* L'insertion est faite en tête de liste */
    v = _hash(key, htable->length);
    item->next = htable->table[v];
    htable->table[v]= item;
  }
  else {
    item->data->value = value;
  }
}


/*----------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/


struct item*
mei_hash_table_lookup(hash_table_t *const htable, const char *const key)
{
  unsigned v;
  struct item *item, *next;   /* Pointers to current and next record
                               * while traversing hash table bucket.  */

  v = _hash(key, htable->length);

  /* Lookup for name in hash table bucket corresponding to above hash value. */

  item = htable->table[v];

  while (item) {
    next = item->next;
    if (!strcmp(item->key, key)) return item;
    item = next;
  }

  return NULL;
}


/*----------------------------------------------------------------------------
 *  Destruction de la table de hachage
 *----------------------------------------------------------------------------*/


void
mei_hash_table_free(hash_table_t *const htable)
{
  int i;
  struct item *item, *next;   /* Pointers to current and next record
                               * while traversing hash table bucket.  */

  if (htable == NULL) return;

  /* Delete hash table as well as data structure representing symbol table. */
  for (i = 0; i < htable->length; i++) {

    item = htable->table[i];

    while (item) {
      next = item->next;
      BFT_FREE(item->key);
      BFT_FREE(item->data);
      BFT_FREE(item);
      item = next;
    }
  }
  BFT_FREE(htable->table);
}


/*----------------------------------------------------------------------------
 *  Fonction qui initialise la table de hachage
 *----------------------------------------------------------------------------*/


void
mei_hash_table_init(hash_table_t *const htable)
{
  int i, j;

  /* predefined constants names*/
  static const char *constants_names[] = { "e", "pi" };

  /* predefined constants values */
  static const double constants[] = {2.7182818284590452354, 3.14159265358979323846};

  /* predefined functions names with one arguments*/
  static const char *functions_names[] = { "exp",  "log",   "sqrt",
                                           "sin",  "cos",   "tan",
                                           "asin", "acos",  "atan",
                                           "sinh", "cosh",  "tanh",
                                           "abs" , "int" };

  /* predefined functions pointers to functions to calculate them */
  static double (*functions[]) (double) = { exp,  log,  sqrt,
                                            sin,  cos,  tan,
                                            asin, acos, atan,
                                            sinh, cosh, tanh,
                                            fabs, floor };

  /* predefined functions names with two arguments*/
  static const char *functions2_names[] = { "atan2", "min", "max", "mod" };

  /* predefined functions pointers to functions to calculate them */
  static double (*functions2[]) (double, double) = { atan2, mei_min, mei_max, fmod };


  j = sizeof(constants_names) / sizeof(constants_names[0]);
  for (i = 0; i < j; i++)
    mei_hash_table_insert(htable,
                          constants_names[i],
                          CONSTANT,
                          constants[i],
                          NULL,
                          NULL,
                          NULL,
                          NULL);

  j = sizeof(functions_names) / sizeof(functions_names[0]);
  for (i = 0; i < j; i++)
    mei_hash_table_insert(htable,
                          functions_names[i],
                          FUNC1,
                          0,
                          functions[i],
                          NULL,
                          NULL,
                          NULL);

  j = sizeof(functions2_names) / sizeof(functions2_names[0]);
  for (i = 0; i < j; i++)
    mei_hash_table_insert(htable,
                          functions2_names[i],
                          FUNC2,
                          0,
                          NULL,
                          functions2[i],
                          NULL,
                          NULL);
}


/*----------------------------------------------------------------------------
 *  Impression des item de la table de hachage pour debugage
 *----------------------------------------------------------------------------*/


void
mei_hash_table_item_print(struct item *item)
{
  /* Affichage d'une liste */
  while (item != NULL) {
    printf("%s -> %i \n", item->key, item->type);
    if (item->type != FUNC1 && item->type != FUNC2 && item->type != FUNC3  && item->type != FUNC4)
      printf("valeur : %f\n", item->data->value);
    item = item->next;
  }
}


/*----------------------------------------------------------------------------
 * Dump of table contents for debuging purpose
 *----------------------------------------------------------------------------*/


void
mei_hash_table_dump(hash_table_t *const htable)
{
  int i;
  for (i = 0; i < htable->length; i++) {
    if (htable->table[i] != NULL) {
      printf("Entrée %d \n", i);
      mei_hash_table_item_print(htable->table[i]);
    }
  }
}


/*============================================================================
 * Test
 *============================================================================*/


#undef _TEST_
#ifdef _TEST_

int
main_test(int argc, char *argv[])
{
#define HASHSIZE 701 // Modulo by default for hash tables (101, 211, 701,...)
  hash_table_t htable;
  struct item *item;
  char *toto = "pi";
  printf("Hash Table Testing\n");

  mei_hash_table_create(&htable, HASHSIZE);
  mei_hash_table_init(&htable);
  mei_hash_table_dump(&htable);
  item = mei_hash_table_lookup(&htable, toto);
  if (item) mei_hash_table_item_print(item);
  mei_hash_table_free(&htable);

  return 0;
}

#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */

