/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: eduardo.j.aguilera
 *
 * Created on 23 de junio de 2016, 8:12
 */

#include <stdio.h>
#include <stdlib.h>

// Librerías incluidas
#include "soapH.h"
#include "httpget.h"
#include "httppost.h"
#include "json_c.h"

/*
 * 
 */
int main(int argc, char** argv) {

    char *proxyHost = NULL;
    
    if (argc == 2)
    {
        printf("Se ha introducido el proxy %s", argv[1]);
        proxyHost = argv[1];
    }
    
    int rc  = llamar_servicio_get(proxyHost);
    rc      = llamar_servicio_post(proxyHost);
    
    
    
    return (EXIT_SUCCESS);
}

int llamar_servicio_get(char *proxyHost)
{
    struct value        resJSON;
    struct soap         *soap           = soap_new();
    
    soap_register_plugin(soap, http_get);
    
    if (proxyHost && strlen(proxyHost) > 0) soap->proxy_host = proxyHost;
        
    printf("Ejemplo de llamada get recuperando JSON\n");
    printf("---------------------------------------\n");
    
    if (soap_get_connect(soap, "http://jsonplaceholder.typicode.com/posts/1", NULL))
    { 
        soap_print_fault(soap, stderr);
        printf("Error en la conexión con la API get.\n");
        goto finalizar;
    }
   
    if (soap_begin_recv(soap))
    {
        soap_print_fault(soap, stderr);
        printf("Error en la llamada a la API get.\n");
        goto finalizar;
    }
   
    if (json_recv(soap, &resJSON))
    {
        soap_print_fault(soap, stderr);
        printf("Error en la obtención del JSON en la API get.\n");
        goto finalizar;
    }
   
    if (soap_end_recv(soap))
    {
        soap_print_fault(soap, stderr);
        printf("Error en la finalización de la API get.\n");
        goto finalizar;
    }
    
    if (resJSON.__type == SOAP_TYPE__struct)
    {
        struct _struct *estructuraJSON;
        estructuraJSON = (struct _struct *)resJSON.ref;
        
        int i;
        for (i=0; i < estructuraJSON->__size; i++)
        {
            printf("%s: ", (estructuraJSON->member + i)->name);
            if ((estructuraJSON->member + i)->value.__type == SOAP_TYPE__string)
            {
                printf("%s\n", (estructuraJSON->member + i)->value.ref);
            }
            else if ((estructuraJSON->member + i)->value.__type == SOAP_TYPE__int)
            {
                printf("%ld\n", (estructuraJSON->member + i)->value.ref);
            }
        }
    }
    else
    {
        printf("Se recibe un JSON no esperado.\n");
        goto finalizar;
    }
    
    printf("Ha finalizado correctamente la llamada get\n");
    printf("------------------------------------------\n");
    
    finalizar:
   
    soap_destroy(soap);
    soap_end(soap);
    soap_free(soap);
    
    return 0;
}

int llamar_servicio_post(char *proxyHost)
{
    struct value        resJSON;
    struct soap         *soap           = soap_new();
    struct value        jsonRequest;
    struct _struct      estructuraPoC;
    struct value        title, body, userId;
    struct member       miembrosPoC[3];
    
    if (proxyHost && strlen(proxyHost) > 0) soap->proxy_host = proxyHost;
    
    printf("Ejemplo de llamada get recuperando JSON\n");
    printf("---------------------------------------\n");
    
    if (soap_post_connect(soap, "http://jsonplaceholder.typicode.com/posts", NULL, "application/json"))
    { 
        soap_print_fault(soap, stderr);
        printf("Error en la conexión con la API post.\n");
        goto finalizar;
    }
    
    // JSON con 3 miembros
    
    // Title
    title.__type        = SOAP_TYPE__string;
    char sTitle[10]     = "foo";
    title.ref           = sTitle;

    // body
    body.__type         = SOAP_TYPE__string;
    char sBody[10]      = "bar";
    body.ref            = sBody;
    
    // userId
    userId.__type       = SOAP_TYPE__int;
    int iUserId         = 1;
    userId.ref          = &iUserId;
    
    miembrosPoC[0].name     = "title";
    miembrosPoC[0].value    = title;
    miembrosPoC[1].name     = "body";
    miembrosPoC[1].value    = body;
    miembrosPoC[2].name     = "userId";
    miembrosPoC[2].value    = userId;
   
    estructuraPoC.__size = 3;
    estructuraPoC.member = miembrosPoC;
    
    jsonRequest.__type   = SOAP_TYPE__struct;
    jsonRequest.ref      = &estructuraPoC;
   
    if (json_send(soap, &jsonRequest))
    {
        soap_print_fault(soap, stderr);
        printf("Error en el envío JSON en la llamada post.\n");
        goto finalizar;
    }
    
    if (soap_end_send(soap))
    {
        soap_print_fault(soap, stderr);
        printf("Error al finalizar envío JSON en la llamada post.");
        goto finalizar;
    }
    
    if (soap_begin_recv(soap))
    {
        soap_print_fault(soap, stderr);
        printf("Error en el inicio de la recepción JSON en la API post.");
        goto finalizar;
    }
   
    if (json_recv(soap, &resJSON))
    {
        soap_print_fault(soap, stderr);
        printf("Error en la obtención del JSON en la API post.\n");
        goto finalizar;
    }
   
    if (soap_end_recv(soap))
    {
        soap_print_fault(soap, stderr);
        printf("Error en la finalización de la API post.\n");
        goto finalizar;
    }
    
    if (resJSON.__type == SOAP_TYPE__struct)
    {
        struct _struct *estructuraJSON;
        estructuraJSON = (struct _struct *)resJSON.ref;
        
        int i;
        for (i=0; i < estructuraJSON->__size; i++)
        {
            printf("%s: ", (estructuraJSON->member + i)->name);
            if ((estructuraJSON->member + i)->value.__type == SOAP_TYPE__string)
            {
                printf("Cadena: %s\n", (estructuraJSON->member + i)->value.ref);
            }
            else if ((estructuraJSON->member + i)->value.__type == SOAP_TYPE__int)
            {
                printf("Entero: %ld\n", *(int*)((estructuraJSON->member + i)->value.ref));
            }
        }
    }
    else
    {
        printf("Se recibe un JSON no esperado.\n");
        goto finalizar;
    }
    
    printf("Ha finalizado correctamente la llamada post\n");
    printf("------------------------------------------\n");
    
    finalizar:
   
    soap_destroy(soap);
    soap_end(soap);
    soap_free(soap);
    
    return 0;
}