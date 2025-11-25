// input.c
// Enable: "Enable Raptor mini (Preview) for all clients"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct client {
    int id;
    bool raptor_mini_enabled; // feature flag per-client
    struct client *next;
} client_t;

static client_t *g_clients = NULL;

// Create and register a client.
client_t *register_client(int id)
{
    client_t *c = (client_t *)malloc(sizeof(client_t));
    if (!c) return NULL;
    c->id = id;
    c->raptor_mini_enabled = false; // default OFF
    c->next = g_clients;
    g_clients = c;
    return c;
}

// Unregister/free all clients (simple cleanup)
void free_clients(void)
{
    client_t *c = g_clients;
    while (c) {
        client_t *n = c->next;
        free(c);
        c = n;
    }
    g_clients = NULL;
}

// Enable Raptor mini (Preview) for all registered clients.
void enable_raptor_mini_for_all_clients(void)
{
    for (client_t *c = g_clients; c; c = c->next) {
        c->raptor_mini_enabled = true;
    }
}

// Query if a client has Raptor mini enabled.
bool is_raptor_mini_enabled_for_client(const client_t *c)
{
    return c ? c->raptor_mini_enabled : false;
}

// Utility: print all clients state
void print_clients_state(void)
{
    for (client_t *c = g_clients; c; c = c->next) {
        printf("Client %d: Raptor mini = %s\n",
               c->id,
               c->raptor_mini_enabled ? "ENABLED" : "DISABLED");
    }
}

// Example usage
#ifdef DEMO_ENABLE_RAPTOR_MINI
int main(void)
{
    register_client(1);
    register_client(2);
    register_client(3);

    puts("Before enabling:");
    print_clients_state();

    enable_raptor_mini_for_all_clients();

    puts("After enabling:");
    print_clients_state();

    free_clients();
    return 0;
}
#endif