#include "config.h"

char *read_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("open failed");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = (char *)malloc(length + 1);
    if (!content)
    {
        perror("malloc failed");
        fclose(file);
        return NULL;
    }
    fread(content, 1, length, file);
    content[length] = '\0';
    fclose(file);
    return content;
}

int parse_config(const char *filename, Config *config)
{
    char *content = read_file(filename);
    if (!content)
    {
        return -1;
    }

    cJSON *json = cJSON_Parse(content);
    if (!json)
    {
        fprintf(stderr, "JSON parse failed: %s\n", cJSON_GetErrorPtr());
        free(content);
        return -1;
    }

    cJSON *client = cJSON_GetObjectItem(json, "client");
    if (client)
    {
        cJSON *ip = cJSON_GetObjectItem(client, "ip");
        if (cJSON_IsString(ip) && ip->valuestring)
        {
            snprintf(config->client.ip, sizeof(config->client.ip), "%s", ip->valuestring);
        }

        cJSON *ports = cJSON_GetObjectItem(client, "ports");
        if (ports)
        {
            cJSON *cmd_port = cJSON_GetObjectItem(ports, "cmd");
            if (cJSON_IsNumber(cmd_port))
            {
                config->client.cmd_port = cmd_port->valueint;
            }

            cJSON *tsf_port = cJSON_GetObjectItem(ports, "tsf");
            if (cJSON_IsNumber(tsf_port))
            {
                config->client.tsf_port = tsf_port->valueint;
            }
        }
    }

    cJSON *thread_pool = cJSON_GetObjectItem(json, "thread_pool");
    if (thread_pool)
    {
        cJSON *tsf_num = cJSON_GetObjectItem(thread_pool, "tsf_num");
        if (cJSON_IsNumber(tsf_num))
        {
            config->thread_pool.tsf_num = tsf_num->valueint;
        }
    }

    cJSON_Delete(json);
    free(content);

    return 0;
}
