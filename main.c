#include <stdio.h>
#include "server.h"

static const char* TAG = "Main";


//Detiene el server
static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

//Manejador del uri
static esp_err_t root_get_handler(httpd_req_t *req)
{
    extern unsigned char view_start[] asm("_binary_view_html_start");
    extern unsigned char view_end[] asm("_binary_view_html_end");
    size_t view_len = view_end - view_start;
    char viewHtml[view_len];

    ESP_LOGI(TAG, "root");

    memcpy(viewHtml, view_start, view_len);
    httpd_resp_send(req, viewHtml, view_len);


    return ESP_OK;
}

static esp_err_t button1_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "buena ahi bro boton1");

    return ESP_OK;
}

static esp_err_t button2_handler(httpd_req_t *req)
{
    
    ESP_LOGI(TAG, "buena ahi bro boton2");

    return ESP_OK;
}

//uri de la pagina inicial 
httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler
};

httpd_uri_t button1 = {
    .uri = "/button1",
    .method = HTTP_GET,
    .handler = button1_handler
};

httpd_uri_t button2 = {
    .uri = "/button2",
    .method = HTTP_GET,
    .handler = button2_handler
};

//Inicia el servidor web
static httpd_handle_t start_webserver(void)
{

    httpd_handle_t server = NULL;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server");

    httpd_config_t conf = HTTPD_DEFAULT_CONFIG();
    esp_err_t ret = httpd_start(&server, &conf);

    if (ESP_OK != ret)
    {
        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &button1);
    httpd_register_uri_handler(server, &button2);

    return server;
}



//Manejadores de eventos para la conexion y desconexion del wifi
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "conectando wifi...\n");
            break;

        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "wifi conectado... \n");
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "conexion perdida... \n");
            break;

        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "IP obtenido... \n\n");
            httpd_handle_t server = start_webserver();
            break;

        default:
            break;

    }
}

// Configuracion y conexion de la red wifi
void wifi_server_setup(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);


    wifi_config_t wifi_configuration = 
    {
        .sta = 
        {
            .ssid = "Familia.L.P", 
            .password = "2352757Jamf"
        }
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);

    esp_wifi_start();

    esp_wifi_connect();

    vTaskDelay(pdMS_TO_TICKS(5000));
}


void app_main(void)
{
    //se inicia la memoria no volatil
    nvs_flash_init();

    //configuracion y conexion del wifi
    wifi_server_setup();

}
