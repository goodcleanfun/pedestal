#ifndef PIPELINE_STEP_H
#define PIPELINE_STEP_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

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

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define STEP_NAME_STR STRINGIFY(STEP_NAME)

#ifndef STEP_OPTIONS_TYPE
typedef OUTPUT_TYPE (*STEP_TYPE(func))(INPUT_TYPE);
#else
typedef OUTPUT_TYPE (*STEP_TYPE(func))(INPUT_TYPE, STEP_OPTIONS_TYPE);
#endif
typedef bool (*STEP_TYPE(validate))(INPUT_TYPE);

typedef struct {
    pipeline_step_t base;
    INPUT_TYPE input;
    OUTPUT_TYPE output;
    #ifdef STEP_OPTIONS_TYPE
    STEP_OPTIONS_TYPE options;
    #endif
    STEP_TYPE(func) func;
    STEP_TYPE(validate) validate;
} STEP_TYPE(step_t);


static bool STEP_TYPE(run)(pipeline_step_t *base, void *data) {
    if (base == NULL) return false;
    if (strcmp(base->type, STEP_NAME_STR) != 0) return false;
    STEP_TYPE(step_t) *step = (STEP_TYPE(step_t) *)base;
    INPUT_TYPE input = (INPUT_TYPE)data;
    step->input = input;
    if (step->validate != NULL && !step->validate(step->input)) return false;
    #ifndef STEP_OPTIONS_TYPE
    step->output = step->func(step->input);
    #else
    step->output = step->func(input, step->options);
    #endif
    return true;
}

static const pipeline_step_t STEP_TYPE(base) = { .type = STEP_NAME_STR, .run = STEP_TYPE(run) };

#ifndef STEP_OPTIONS_TYPE
STEP_TYPE(step_t) *STEP_TYPE(new)(STEP_TYPE(func) func, STEP_TYPE(validate) validate) {
#else
STEP_TYPE(step_t) *STEP_TYPE(new)(STEP_TYPE(func) func, STEP_TYPE(validate) validate, STEP_OPTIONS_TYPE options) {
#endif
    STEP_TYPE(step_t) *step = malloc(sizeof(STEP_TYPE(step_t)));
    if (step == NULL) return NULL;
    step->base = STEP_TYPE(base);
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
#undef STRINGIFY_
#undef STRINGIFY
#undef STEP_NAME_STR
