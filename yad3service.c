#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "yad3service.h"

struct Yad3Service_t {
    Map realtorsService;
    Map customersService;
};
struct Realtor_t {
	char* companyName;
	int taxPercentage;
	Map apartmentServices;
};

struct Customer_t {
	int min_area;
	int min_rooms;
	int max_price;
	int money_spent;
	Set customerOffers;
};

struct Offer_t {
	char* realtor_email;
	char* service_name;
	int apartment_id;
	int new_price;
	int status;
};

/*============================================================================*/
/* General Helper functions for Yad3Service */
/*============================================================================*/

static char* stringDuplicate(const char* str) {
	assert(str);
	char* result = malloc(strlen(str)+1);
	return result ? strcpy(result,str) : NULL;
}

/*============================================================================*/
/* General Helper functions for map */
/*============================================================================*/

int compareMapKey(constMapKeyElement key1, constMapKeyElement key2) {
	return strcmp(key1, key2);
}

static bool validateEmail(char* email) {
	//assert(email);
	return strstr(email, "@") != NULL ? true : false;
}

static bool validatePercentage (int percentage) {
	return percentage > 0 && percentage <101 ? true : false;
}

static bool emailAlreadyExist (Yad3Service service, char* email, User user) {
	return mapContains(service->realtorsService, email);
}

static bool apartmentServiceAlreadyExist(Map services, char* service_name) {
	assert(services);
	return mapContains(services, service_name);
}

/*============================================================================*/
/* Map functions for realtor & customer maps*/
/*============================================================================*/

MapKeyElement copyEmail(constMapKeyElement email) {
	char* key = (char*)email;
	return stringDuplicate(key);
}

void freeEmail(MapKeyElement key) {
	free(key);
}

/*============================================================================*/
/* Realtor map functions */
/*============================================================================*/

MapDataElement copyRealtorStruct(constMapDataElement realtor){
	Realtor tmp_realtor = (Realtor)realtor;
	Realtor new_realtor = malloc(sizeof(*new_realtor));
	if(new_realtor == NULL){
		return NULL;
	}
	new_realtor->companyName = stringDuplicate(tmp_realtor->companyName);
	new_realtor->taxPercentage = tmp_realtor->taxPercentage;
	new_realtor->apartmentServices = mapCopy(tmp_realtor->apartmentServices);
	return new_realtor;
}

void freeRealtorStruct(MapDataElement realtor) {
	Realtor tmp_realtor = (Realtor)realtor;
	mapDestroy(tmp_realtor->apartmentServices);
	free(tmp_realtor->companyName);

	free(tmp_realtor);
}

/*============================================================================*/
/* apartmentService map functions */
/*============================================================================*/

MapDataElement copyApartmentService(constMapDataElement service){
	ApartmentService new_service = (ApartmentService)service;
	return serviceCopy(new_service);
}

MapKeyElement copyApartmentServiceName(constMapKeyElement service_name) {
	return stringDuplicate(service_name);
}

void freeApartmentService(MapDataElement service) {
	serviceDestroy(service);
}

void freeApartmentServiceName(MapKeyElement key) {
	free(key);
}

/*============================================================================*/
/* yad3Service functions */
/*============================================================================*/

Yad3Service yad3ServiceCreate() {
	Yad3Service service = malloc(sizeof(*service));
	Map realtors_map = mapCreate(copyRealtorStruct, copyEmail,
				freeRealtorStruct, freeEmail, compareMapKey);
	service->realtorsService = realtors_map;
	service->customersService = realtors_map;
	return service;
}

void yad3ServiceDestroy(Yad3Service service) {


	mapDestroy(service->realtorsService);
	// UNCOMMENT THIS WHEN CUSTOMERSSERVICE IS DONE
	//mapDestroy(service->customersService);
	free(service);
}

Yad3Result yad3AddRealtor(Yad3Service service, char* email, char* company_name,
		int tax_percentage) {
	if (emailAlreadyExist(service, email, REALTOR) == true) {
		return YAD3_EMAIL_ALREADY_EXISTS;
	}
	Realtor new_realtor = malloc(sizeof(*new_realtor));
	Map new_map = mapCreate(copyApartmentService, copyApartmentServiceName,
			freeApartmentService, freeApartmentServiceName, compareMapKey);

	if (new_realtor == NULL || new_map == NULL) {
		free(new_realtor);
		mapDestroy(new_map);
		return YAD3_OUT_OF_MEMORY;
	}

	if (!validateEmail(email) || !validatePercentage(tax_percentage)) {
	    return YAD3_INVALID_PARAMETERS;
	}

	new_realtor->companyName = stringDuplicate(company_name);
	new_realtor->taxPercentage = tax_percentage;
	new_realtor->apartmentServices = new_map;
	mapPut(service->realtorsService, email, new_realtor);

	mapDestroy(new_map);
	free(new_realtor->companyName);
	free(new_realtor);
	return YAD3_SUCCESS;
}

Yad3Result yad3DeleteRealtor(Yad3Service service, char* key) {
	if (service == NULL) {
		return YAD3_OUT_OF_MEMORY;
	}
	if (validateEmail(key) == false) {
		return YAD3_INVALID_PARAMETERS;
	}
	mapRemove(service->realtorsService, (char*)key);
	return YAD3_SUCCESS;
}

Yad3Result yad3AddApartmentService(Yad3Service service, char* realtor_email,
	char* service_name, int max_size) {
//	assert(realtor_email && service_name && max_size);

	if (emailAlreadyExist(service, realtor_email, REALTOR) == false) {
		return YAD3_EMAIL_DOES_NOT_EXIST;
	}
	Realtor realtor = mapGet(service->realtorsService, realtor_email);
	if (apartmentServiceAlreadyExist(realtor->apartmentServices, service_name) == true) {
		return YAD3_APARTMENT_SERVICE_ALREADY_EXISTS;
	}
	// WHAT TO DO ABOUT MAX_SIZE = 0?
	if (max_size < 1) {
		return YAD3_INVALID_PARAMETERS;
	}

	ApartmentService apartment_service = serviceCreate(max_size);
	if (service == NULL || realtor == NULL || apartment_service == NULL) {
		return YAD3_OUT_OF_MEMORY;
	}
	if (!validateEmail(realtor_email)) {
		return YAD3_INVALID_PARAMETERS;
	}
	Realtor tmp = (Realtor)realtor;
	Map current_map = tmp->apartmentServices;
	char* new_service_name = stringDuplicate(service_name);
	mapPut(current_map, new_service_name, apartment_service);
	free(new_service_name);
	serviceDestroy(apartment_service);

	return YAD3_SUCCESS;
}

Yad3Result yad3RemoveApartmentService(Yad3Service service,
		char* realtor_email, char* service_name) {
	if (service == NULL) {
		return YAD3_OUT_OF_MEMORY;
	}
	Realtor realtor = mapGet(service->realtorsService, (char*)realtor_email);
	if (apartmentServiceAlreadyExist(realtor->apartmentServices ,service_name) == false) {
		return YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST;
	}
	if (!validateEmail(realtor_email)) {
		return YAD3_INVALID_PARAMETERS;
	}
	if (realtor == NULL) {
		return YAD3_EMAIL_DOES_NOT_EXIST;
	}
	MapResult result = mapRemove(realtor->apartmentServices, (char*)service_name);
	if (result == MAP_ITEM_DOES_NOT_EXIST) {
		return YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST;
	}
	return YAD3_SUCCESS;
}
