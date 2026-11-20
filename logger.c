#include <stdio.h>
#include "logger.h"

int log_message(LogLevel level, const char *message, const char *argv1)
{
    if (message == NULL)
        return 1;

    const char *tmplev;

    switch (level)
    {
        case INFO:
            tmplev = "INFO";
            break;

        case WARNING:
            tmplev = "WARNING";
            break;

        case ERROR:
            tmplev = "ERROR";
            break;

        case CRITICAL:
            tmplev = "CRITICAL";
            break;

        default:
            return 1;
    }

    char tmpmess[1024];

    int len = snprintf(
        tmpmess,
        sizeof(tmpmess),
        "[%s] - { %s }",
        tmplev,
        message
    );

    if (len < 0 || (size_t)len >= sizeof(tmpmess))
        return 1;

    if (argv1 != NULL)
    {
        FILE *logf = fopen(argv1, "a");

        if (logf == NULL)
            return 1;

        int result = fprintf(logf, "%s\n", tmpmess);
        int close_result = fclose(logf);

        if (result < 0 || close_result != 0)
            return 1;
    }
    else
    {
        printf("%s\n", tmpmess);
    }

    return 0;
}