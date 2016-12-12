
#ifndef __TOKENIZE_H__
#define __TOKENIZE_H__

#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include "types.h"
#include "util/khash.h"
#include "varint.h"
#include "stemmer.h"

typedef enum {
    DT_WORD,
    DT_STEM,
} DocTokenType;
/* Represents a token found in a document */
typedef struct {
    // token string
    const char *s;
    // token string length
    size_t len;

    // position in the document - this is written to the inverted index
    u_int pos;

    // the token's score
    float score;

    // Field id - used later for filtering.
    u_char fieldId;

    int stringFreeable;

    DocTokenType type;
} Token;

// A TokenFunc handles tokens in a tokenizer, for example aggregates them, or builds the query tree
typedef int (*TokenFunc)(void *ctx, Token t);

// A NormalizeFunc converts a raw token to the normalized form in which it will be stored
typedef char *(*NormalizeFunc)(char *, size_t *);

//! " # $ % & ' ( ) * + , - . / : ; < = > ? @ [ \ ] ^ _ ` { | } ~
#define DEFAULT_SEPARATORS " \t,./(){}[]:;/\\~!@#$%^&*-_=+|'`\"<>?";
static const char *stopwords[] = {
    "a",    "is",    "the",   "an",   "and",  "are", "as",  "at",   "be",   "but",  "by",   "for",
    "if",   "in",    "into",  "it",   "no",   "not", "of",  "on",   "or",   "such", "that", "their",
    "then", "there", "these", "they", "this", "to",  "was", "will", "with", NULL};

#define STEM_TOKEN_FACTOR 0.2

// TODO: Optimize this with trie or something...
int isStopword(const char *w);

typedef struct {
    const char *text;
    char **pos;
    const char *separators;
    double fieldScore;
    int fieldId;
    TokenFunc tokenFunc;
    void *tokenFuncCtx;
    NormalizeFunc normalize;
    Stemmer *stemmer;
} TokenizerCtx;

/* The actual tokenizing process runner */
int _tokenize(TokenizerCtx *ctx);

/** The extenral API. Tokenize text, and create tokens with the given score and fieldId.
TokenFunc is a callback that will be called for each token found
if doStem is 1, we will add stemming extraction for the text
*/
int tokenize(const char *text, float fieldScore, u_char fieldId, void *ctx, TokenFunc f,
             Stemmer *s);

/** A simple text normalizer that convertes all tokens to lowercase and removes accents.
Does NOT normalize unicode */
char *DefaultNormalize(char *s, size_t *len);

#endif