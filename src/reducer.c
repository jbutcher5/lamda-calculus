#include "reducer.h"
#include "parser.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_ITER_DEPTH 64

void _apply(LambdaContent *lambda, LambdaContent *parent, Node *argument) {
  for (int i = 0; i < lambda->body.size; i++) {
    Node *node = lambda->body.ast + i;

    if (node->type == Parameter) {
      ParameterContent *parameter = (ParameterContent *)node->content;

      if (!parameter->value &&
          (parameter->parent == parent || parameter->parent == NULL)) {
        free_node(node);
        *node = clone_node(*argument);
      }

      else if (parameter->parent == parent) {
        parameter->value--;
      }
    }

    else if (node->type == Lambda) {
      _apply((LambdaContent *)node->content, parent, argument);
    }
  }

  if (lambda == parent) {
    lambda->parameter_number--;
    lambda->parameters++;

    // TODO: First element in lambda->parameters is leaked here. Solve later ig
  }
}

void apply(LambdaContent *lambda, Node *argument) {
  _apply(lambda, lambda, argument);
}

int beta_reduction(Expr *expr) {
  static Expr *last_expr = NULL;
  static int iteration = 0;

  if (!last_expr)
    last_expr = expr;

  for (int i = 0; i < expr->size; i++) {
    Node *node = expr->ast + i;

    if (node->type == Lambda && i < expr->size - 1) {
      LambdaContent *lambda = node->content;

      apply(node->content, node + 1);
      free_node(node + 1);

      if (!lambda->parameter_number) {
        if (lambda->body.size == 2) {
          *node = lambda->body.ast[0];
          *(node + 1) = lambda->body.ast[1];

          // free(lambda->parameters);
          free(lambda->body.ast);

          // TODO: Check each item of the ast is freed
        } else {
          int new_size = expr->size - 2 + lambda->body.size;
          Node *new_ast = calloc(sizeof(Node), new_size);

          // TODO: All of these nodes we pass over need freed

          for (int j = 0; j < i; j++) {
            new_ast[j] = expr->ast[j];
          }

          for (int j = i, k = 0; k < lambda->body.size; j++, k++) {
            new_ast[j] = lambda->body.ast[k];
          }

          for (int j = i + lambda->body.size, k = i + 2; k < expr->size;
               j++, k++) {
            new_ast[j] = expr->ast[k];
          }

          expr->ast = new_ast;
          expr->size = new_size;
        }
      }

      if (last_expr == expr && iteration > MAX_ITER_DEPTH) {
        return 0;
      } else if (last_expr == expr) {
        iteration++;
      } else {
        iteration = 0;
        last_expr = expr;
      }

      return 1;
    }
  }

  return 0;
}
