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

    cJSON *server = cJSON_GetObjectItem(json, "server");
    if (server)
    {
        cJSON *ip = cJSON_GetObjectItem(server, "ip");
        if (cJSON_IsString(ip) && ip->valuestring)
        {
            snprintf(config->server.ip, sizeof(config->server.ip), "%s", ip->valuestring);
        }

        cJSON *ports = cJSON_GetObjectItem(server, "ports");
        if (ports)
        {
            cJSON *cmd_port = cJSON_GetObjectItem(ports, "cmd");
            if (cJSON_IsNumber(cmd_port))
            {
                config->server.cmd_port = cmd_port->valueint;
            }

            cJSON *tsf_port = cJSON_GetObjectItem(ports, "tsf");
            if (cJSON_IsNumber(tsf_port))
            {
                config->server.tsf_port = tsf_port->valueint;
            }
        }
    }

    cJSON *thread_pool = cJSON_GetObjectItem(json, "thread_pool");
    if (thread_pool)
    {
        cJSON *cmd_num = cJSON_GetObjectItem(thread_pool, "cmd_num");
        if (cJSON_IsNumber(cmd_num))
        {
            config->thread_pool.cmd_num = cmd_num->valueint;
        }

        cJSON *tsf_num = cJSON_GetObjectItem(thread_pool, "tsf_num");
        if (cJSON_IsNumber(tsf_num))
        {
            config->thread_pool.tsf_num = tsf_num->valueint;
        }
    }

    cJSON *mysql = cJSON_GetObjectItem(json, "mysql");
    if (mysql)
    {
        cJSON *ip = cJSON_GetObjectItem(mysql, "ip");
        if (cJSON_IsString(ip) && ip->valuestring)
        {
            snprintf(config->mysql.ip, sizeof(config->mysql.ip), "%s", ip->valuestring);
        }

        cJSON *port = cJSON_GetObjectItem(mysql, "port");
        if (cJSON_IsNumber(port))
        {
            config->mysql.port = port->valueint;
        }

        cJSON *username = cJSON_GetObjectItem(mysql, "username");
        if (cJSON_IsString(username) && username->valuestring)
        {
            snprintf(config->mysql.username, sizeof(config->mysql.username), "%s", username->valuestring);
        }

        cJSON *password = cJSON_GetObjectItem(mysql, "password");
        if (cJSON_IsString(password) && password->valuestring)
        {
            snprintf(config->mysql.password, sizeof(config->mysql.password), "%s", password->valuestring);
        }

        cJSON *database = cJSON_GetObjectItem(mysql, "database");
        if (cJSON_IsString(database) && database->valuestring)
        {
            snprintf(config->mysql.database, sizeof(config->mysql.database), "%s", database->valuestring);
        }
    }

    cJSON_Delete(json);
    free(content);

    return 0;
}

