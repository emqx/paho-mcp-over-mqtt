#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/mcp.h"
#include "../include/mcp_server.h"

void signal_handler(int sig)
{
    (void) sig;
}

const char *add_numbers(int n_args, property_t *args)
{
    (void) n_args;
    int a = args[0].value.integer_value;
    int b = args[1].value.integer_value;

    int result = a + b;

    char *response = malloc(32);
    snprintf(response, 32, "%d", result);
    return response;
}

const char *get_temperature_callback(int n_args, property_t *args)
{
    // Read sensor data
    float temp = 25.5;

    // Return JSON formatted result
    static char result[64];
    snprintf(result, sizeof(result), "{\"temperature\": %.2f}", temp);
    return result;
}

const char *array_function(int n_args, property_t *args)
{
    (void) n_args;
    property_t array_prop = args[0];
    if (array_prop.type != PROPERTY_ARRAY) {
        return "Invalid argument type";
    }

    long long sum = 0;
    for (int i = 0; i < array_prop.n_elements; i++) {
        property_t element = array_prop.elements[i];
        if (element.type != PROPERTY_INTEGER) {
            return "Invalid array element type";
        }
        sum += element.value.integer_value;
    }

    char *response = malloc(32);
    snprintf(response, 32, "%lld", sum);
    return response;
}

const char *object_function(int n_args, property_t *args)
{
    (void) n_args;
    property_t object_prop = args[0];
    if (object_prop.type != PROPERTY_OBJECT) {
        return "Invalid argument type";
    }

    char * mode      = NULL;
    double threshold = 0.0;

    for (int i = 0; i < object_prop.n_elements; i++) {
        property_t element = object_prop.elements[i];
        if (strcmp(element.name, "mode") == 0 &&
            element.type == PROPERTY_STRING) {
            mode = element.value.string_value;
        } else if (strcmp(element.name, "threshold") == 0 &&
                   element.type == PROPERTY_REAL) {
            threshold = element.value.real_value;
        }
    }

    char *response = malloc(128);
    snprintf(response, 128, "Mode: %s, Threshold: %.2f", mode, threshold);
    return response;
}

mcp_tool_t tools[] = {
    {
        .name           = "add",
        .description    = "Adds two numbers",
        .call           = add_numbers,
        .property_count = 2,
        .properties =
            (property_t[]) {
                {
                    .name                = "a",
                    .description         = "First number",
                    .type                = PROPERTY_INTEGER,
                    .value.integer_value = 0,
                },
                {
                    .name                = "b",
                    .description         = "Second number",
                    .type                = PROPERTY_INTEGER,
                    .value.integer_value = 0,
                },
            },
    },
    {
        .name           = "get_temperature",
        .description    = "Get device temperature",
        .property_count = 0,
        .properties     = NULL,
        .call           = get_temperature_callback,
    },
    {
        .name           = "array_function",
        .description    = "Function with array argument",
        .property_count = 1,
        .properties =
            (property_t[]) {
                {
                    .name        = "values",
                    .description = "Array of numbers",
                    .type        = PROPERTY_ARRAY,
                    .n_elements  = 3,
                    .elements =
                        (property_t[]) {
                            {
                                .name                = NULL,
                                .description         = NULL,
                                .type                = PROPERTY_INTEGER,
                                .value.integer_value = 0,
                            },
                            {
                                .name                = NULL,
                                .description         = NULL,
                                .type                = PROPERTY_INTEGER,
                                .value.integer_value = 0,
                            },
                            {
                                .name                = NULL,
                                .description         = NULL,
                                .type                = PROPERTY_INTEGER,
                                .value.integer_value = 0,
                            },
                        },
                },
            },
        .call = array_function,
    },
    {
        .name           = "object_function",
        .description    = "Function with object argument",
        .property_count = 1,
        .properties =
            (property_t[]) {
                {
                    .name        = "config",
                    .description = "Configuration object",
                    .type        = PROPERTY_OBJECT,
                    .n_elements  = 2,
                    .elements =
                        (property_t[]) {
                            {
                                .name        = "mode",
                                .description = "Operating mode",
                                .type        = PROPERTY_STRING,
                            },
                            {
                                .name        = "threshold",
                                .description = "Threshold value",
                                .type        = PROPERTY_REAL,
                            },
                        },
                },
            },
        .call = object_function,
    },
};

void mcp_server_example()
{
    mcp_server_t *server = mcp_server_init(
        "ESP32 Demo Server Name", "This is an example MCP server",
        "tcp://broker.fengzero.com:1883", "example_client", NULL, NULL, NULL);

    mcp_server_register_tool(server, 4, tools);

    mcp_server_run(server);
}

int main()
{
    mcp_server_example();

    while (1) {
        sleep(1);
    }

    return 0;
}
