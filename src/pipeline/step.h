#ifndef PIPELINE_STEP_H
#define PIPELINE_STEP_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct pipeline_step {
    char *type;
    bool (*run)(struct pipeline_step *step, void *data);
} pipeline_step_t;

#endif // PIPELINE_STEP_H - this part only gets included once, specific instances include this multiple times

#ifndef STEP_NAME
#error "STEP_NAME must be defined"
#endif

#ifndef INPUT_TYPE
#error "INPUT_TYPE must be defined"
#endif

#ifndef OUTPUT_TYPE
#error "OUTPUT_TYPE must be defined"
#endif

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)
#define STEP_TYPE(name) CONCAT(STEP_NAME, _##name)

typedef OUTPUT_TYPE (*STEP_TYPE(func))(INPUT_TYPE);
typedef bool (*STEP_TYPE(validate))(INPUT_TYPE);

typedef struct {
    pipeline_step_t *head;
    INPUT_TYPE input;
    OUTPUT_TYPE output;
    STEP_TYPE(func) func;
    STEP_TYPE(validate) validate;
} STEP_TYPE(step_t);


static bool STEP_TYPE(run)(step_t *base, void *data) {
    if (base == NULL) return false;
    if (base->type != STEP_NAME) return false;
    if (base->validate != NULL && !base->validate(((STEP_TYPE(step_t) *)base)->input)) return false;
    STEP_TYPE(step_t) *step = (STEP_TYPE(step_t) *)base;
    INPUT_TYPE input = (INPUT_TYPE)data;
    step->input = input;
    step->output = step->func(input);
    return true;
}

static step_t STEP_TYPE(head) = { .type = STEP_NAME, .run = STEP_TYPE(run) };

STEP_TYPE(step_t) *STEP_TYPE(new)(STEP_TYPE(func) func, STEP_TYPE(validate) validate) {
    STEP_TYPE(step_t) *step = malloc(sizeof(STEP_TYPE(step_t)));
    if (step == NULL) return NULL;
    step->head = &STEP_TYPE(head);
    step->input = (INPUT_TYPE)0;
    step->output = (OUTPUT_TYPE)0;
    step->validate = validate;
    step->func = func;
    return step;
}

void STEP_TYPE(destroy)(STEP_TYPE(step_t) *step) {
    if (step == NULL) return;
    free(step);
}

#undef CONCAT_
#undef CONCAT
#undef STEP_TYPE
