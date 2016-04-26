#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "yad3service.h"
#include "mtm_ex2.h"

typedef void* Element;

struct Yad3Service_t {
	Map realtorsService;
	Map customersService;
};

struct Realtor_t {
	char* companyName;
	int taxPercentage;
	Map apartmentServices;
	long long int rank;
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
	Choice status;
};

/*============================================================================*/
/* General Helper functions for Yad3Service */
/*============================================================================*/

static char* stringDuplicate(const char* str) {
	assert(str);
	char* result = malloc(strlen(str) + 1);
	return result ? strcpy(result, str) : NULL;
}

void printRealtorData(Map realtors) {
	MAP_FOREACH(MapKeyElement,iterator,realtors)
	{
		Realtor getElement = mapGet(realtors, (char*) iterator);
		char* companyName = (char*) getElement->companyName;
		mtmPrintRealtor(stdout, (char*) iterator, companyName);
	}
}

void printApartment(Yad3Service service, char* email, char* service_name,
		int id) {
	Map realtor_map = service->realtorsService;
	Realtor current_realtor = (Realtor) mapGet(realtor_map, email);
	Map all_apartment_services = current_realtor->apartmentServices;
	ApartmentService apartment_service = (ApartmentService) mapGet(
			all_apartment_services, service_name);

	Apartment apat;
	ApartmentServiceResult res = serviceGetById(apartment_service, id, &apat); // PROBLEM IS HERE.
	if (res != APARTMENT_SERVICE_SUCCESS) {
		printf("problem id: %d fetching the Apartment\n", (int) res);
		return;
	}
	int height = apartmentGetLength(apat);
	int width = apartmentGetWidth(apat);

	SquareType out;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			apartmentGetSquare(apat, i, j, &out);
			printf("%c ", (out == EMPTY) ? 'e' : 'w');
		}
		printf("\n");
	}
}

/*============================================================================*/
/* General Helper functions for map */
/*============================================================================*/

int compareMapKey(constMapKeyElement key1, constMapKeyElement key2) {
	assert(key1 && key2);
	return strcmp(key1, key2);
}


static bool validateEmail(char* email) {
	//assert(email);
	return strstr(email, "@") != NULL ? true : false;
}

static bool validatePercentage(int percentage) {
	return percentage > 0 && percentage < 101 ? true : false;
}

static bool emailAlreadyExist(Yad3Service service, char* email, User user) {
	return mapContains(service->realtorsService, email);
}

static bool apartmentServiceAlreadyExist(Map services, char* service_name) {
	assert(services);
	return mapContains(services, service_name);
}

static void convertMapToArrays(Map realtors, long long int* rank_array,
		char** email_array) {
	int i = 0;
	Realtor realtor;
	MAP_FOREACH(MapKeyElement, email, realtors)
	{
		realtor = mapGet(realtors, email);
		rank_array[i] = realtor->rank;
		email_array[i++] = (char*) email;
//		printf("email_array[%d]: %s\n", i - 1, email_array[i - 1]);
	}
}

static void arrayDestroy(char** array, int size) {
	if (array == NULL) {
		return;
	}
	for (int i = 0; i < size; i++) {
		free(array[i]);
	}
	free(array);
}

static void swap(Element* element1, Element* element2) {
	Element temp = *element1;
	*element1 = *element2;
	*element2 = temp;
}

static void bubbleSort(long long int* rank_array, char** email_array, int size) {
	for (int i = 0; i < size - 1; i++) {
		for (int j = 0; j < size - i - 1; j++) {
			if (rank_array[j] < rank_array[j + 1]) {
				long long int temp = rank_array[j];
				rank_array[j] = rank_array[j + 1];
				rank_array[j + 1] = temp;
				swap((Element*) &email_array[j],
						(Element*) &email_array[j + 1]);
			}
			if ((rank_array[j] == rank_array[j + 1])
					&& strcmp(email_array[j], email_array[j + 1]) > 0) {
				swap((Element*) &email_array[j],
						(Element*) &email_array[j + 1]);
			}
		}
	}
//	for (int i = 0; i < size; i++) {
//		printf("rank_array[%d] = %lld, email_array[%d]: %s\n", i, rank_array[i],
//				i, email_array[i]);
//	}
}

/*============================================================================*/
/* Map functions for realtor & customer maps*/
/*============================================================================*/

MapKeyElement copyEmail(constMapKeyElement email) {
	char* key = (char*) email;
	return stringDuplicate(key);
}

void freeEmail(MapKeyElement key) {
	free(key);
}

static Yad3Result checkEmailExists(Yad3Service service, char* customer_email,
		char* realtor_email) {
	bool customer_exists = mapContains(service->customersService,
			customer_email);
	bool realtor_exists = mapContains(service->realtorsService, realtor_email);
	bool customer_is_realtor = mapContains(service->realtorsService,
			customer_email);
	bool realtor_is_customer = mapContains(service->customersService,
			realtor_email);
	if (!customer_exists || !realtor_exists) {
		return YAD3_EMAIL_DOES_NOT_EXIST;
	}
	if (customer_is_realtor || realtor_is_customer) {
		return YAD3_EMAIL_WRONG_ACCOUNT_TYPE;
	}
	return YAD3_SUCCESS;
}

static Yad3Result checkRealtorHasApartment(Realtor realtor,
		char* customer_email, char* realtor_email, char* service_name,
		int apartment_id, Apartment* apartment) {
	if (!mapContains(realtor->apartmentServices, service_name)) {
		return YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST;
	}
	ApartmentService apartment_service = mapGet(realtor->apartmentServices,
			service_name);
	ApartmentServiceResult apartment_exists = serviceGetById(apartment_service,
			apartment_id, apartment);
	if (apartment_exists != APARTMENT_SERVICE_SUCCESS) {
		return YAD3_APARTMENT_DOES_NOT_EXIST;
	}
	return YAD3_SUCCESS;
}

static bool apartmentParametersMatch(Apartment apartment, Customer customer) {
	int area = apartmentTotalArea(apartment);
	int num_of_rooms = apartmentNumOfRooms(apartment);
	int price = apartmentGetPrice(apartment);
	if ((area < customer->min_area) || (num_of_rooms < customer->min_rooms)
			|| (price > customer->max_price)) {
		return false;
	}
	return true;
}

/*============================================================================*/
/* Realtor map functions */
/*============================================================================*/

MapDataElement copyRealtorStruct(constMapDataElement realtor) {
	Realtor tmp_realtor = (Realtor) realtor;
	Realtor new_realtor = malloc(sizeof(*new_realtor));
	if (new_realtor == NULL) {
		return NULL;
	}
	new_realtor->companyName = stringDuplicate(tmp_realtor->companyName);
	new_realtor->taxPercentage = tmp_realtor->taxPercentage;
	new_realtor->apartmentServices = mapCopy(tmp_realtor->apartmentServices);
	return new_realtor;
}

void freeRealtorStruct(MapDataElement realtor) {
	if (realtor == NULL) {
		return;
	}
	Realtor tmp_realtor = (Realtor) realtor;
	free(tmp_realtor->companyName);
	mapDestroy(tmp_realtor->apartmentServices);
	free(tmp_realtor);
}

static long long int calculateRealtorRank(Realtor realtor) {
	int apartments, price_median, area_median;
	int apartments_tot = 0;
	int price_tot = 0;
	int area_tot = 0;
	int services = 0;

	if (realtor->apartmentServices == NULL) {
		return -1;
	}
	MAP_FOREACH(MapKeyElement, service_id, realtor->apartmentServices)
	{
		ApartmentService service = mapGet(realtor->apartmentServices,
				service_id);
		if (service == NULL) {
			return -1;
		}
		apartments = serviceNumberOfApatments(service);
		if (apartments > 1) {
			servicePriceMedian(service, &price_median);
			serviceAreaMedian(service, &area_median);
			services++;
		}
		apartments_tot += apartments;
		price_tot += price_median;
		area_tot += area_median;
	}
	if (services == 0) {
		return 0;
	}
	return (1000000 * apartments_tot) + (int) (price_tot / services)
			+ (100000 * (int) (area_tot / services));
}

/*============================================================================*/
/* Customer map functions */
/*============================================================================*/

MapDataElement copyCustomerStruct(constMapDataElement customer) {
	Customer tmp_customer = (Customer) customer;
	Customer new_customer = malloc(sizeof(*new_customer));
	if (new_customer == NULL) {
		return NULL;
	}
	new_customer->min_area = tmp_customer->min_area;
	new_customer->min_rooms = tmp_customer->min_rooms;
	new_customer->max_price = tmp_customer->max_price;
	new_customer->money_spent = tmp_customer->money_spent;
	new_customer->customerOffers = setCopy(tmp_customer->customerOffers);
	return new_customer;
}

void freeCustomerStruct(MapDataElement customer) {
	if (customer == NULL) {
		return;
	}
	Customer tmp_customer = (Customer) customer;
	setDestroy(tmp_customer->customerOffers);
	free(tmp_customer);
}

/*============================================================================*/
/* Offer set functions */
/*============================================================================*/

SetElement copyOffer(SetElement offer) {
	Offer copy = malloc(sizeof(*copy));
	if (copy == NULL) {
		return NULL;
	}
	Offer original = (Offer) offer;
	copy->realtor_email = stringDuplicate(original->realtor_email);
	copy->service_name = stringDuplicate(original->service_name);
	copy->apartment_id = original->apartment_id;
	copy->new_price = original->new_price;
	copy->status = original->status;
	return copy;
}

void freeOffer(SetElement offer) {
	if (offer == NULL) {
		return;
	}
	Offer original = (Offer) offer;
	free(original->realtor_email);
	free(original->service_name);
	free(original);
}

int compareOffers(SetElement offer1, SetElement offer2) {
	Offer original1 = (Offer) offer1;
	Offer original2 = (Offer) offer2;
	if (original1->new_price > original2->new_price) {
		return 1;
	}
	if (original1->new_price < original2->new_price) {
		return -1;
	}
	return 0;
}

static bool isSameOffer(Offer offer, char* realtor_email, char* service_name,
		int apartment_id) {
	if ((offer->realtor_email == realtor_email)
			&& (offer->service_name == service_name)
			&& (offer->apartment_id == apartment_id)) {
		return true;
	}
	return false;
}
static bool alreadySentOffer(Customer customer, char* realtor_email) {
	SET_FOREACH(SetElement, offer, customer->customerOffers)
	{
		Offer current = (Offer) offer;
		if (current->realtor_email == realtor_email) {
			return true;
		}
	}
	return false;
}

static void removeAllOffersOnApartment(Yad3Service service,
		char* customer_email, char* realtor_email, char* service_name,
		int apartment_id) {
	MAP_FOREACH(MapKeyElement, customer_email, service->customersService)
	{
		Customer some_customer = mapGet(service->customersService,
				customer_email);
		SET_FOREACH(SetElement, offer, some_customer->customerOffers)
		{
			if (isSameOffer((Offer) offer, realtor_email, service_name,
					apartment_id)) {
				setRemove(some_customer->customerOffers, offer);
			}
		}
	}
}
static int calculateFinalPrice(Apartment apartment, int tax_percentage) {
	int price_before_tax = apartmentGetPrice(apartment);
	return (tax_percentage / 100 * price_before_tax) + price_before_tax;

}

static Yad3Result createOffer(Customer customer, char* realtor_email,
		char* service_name, int apartment_id, int new_price) {
	Offer offer = malloc(sizeof(*offer));
	if (offer == NULL) {
		return YAD3_OUT_OF_MEMORY;
	}
	offer->realtor_email = stringDuplicate(realtor_email);
	offer->service_name = stringDuplicate(service_name);
	offer->apartment_id = apartment_id;
	offer->new_price = new_price;
	offer->status = PENDING;
	return YAD3_SUCCESS;
}

/*============================================================================*/
/* apartmentService map functions */
/*============================================================================*/

MapDataElement copyApartmentService(constMapDataElement service) {
	ApartmentService new_service = (ApartmentService) service;
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

static SquareType** createMatrixGrid(char* matrix, int length, int width) {
	SquareType** grid = malloc(length * sizeof(SquareType*));
	if (grid == NULL) {
		return NULL;
	}
	for (int i = 0; i < length; i++) {
		grid[i] = malloc(width * sizeof(SquareType));
		if (grid[i] == NULL) {
			for (int j = 0; j < i; j++) {
				free(grid[j]);
			}
			return NULL;
		}
	}
	int c = 0;
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < width; j++) {
			grid[i][j] = matrix[c++] == 'e' ? EMPTY : WALL;
		}
	}
	return grid;
}

static void destroyMatrixGrid(SquareType** grid, int length) {
	if (grid == NULL) {
		return;
	}
	for (int i = 0; i < length; i++) {
		SquareType* currentPtr = grid[i];
		free(currentPtr);
		currentPtr = NULL;
	}
	free(grid);
}

/*============================================================================*/
/* yad3Service functions */
/*============================================================================*/

Yad3Service yad3ServiceCreate() {
	Yad3Service service = malloc(sizeof(*service));
	Map realtors_map = mapCreate(copyRealtorStruct, copyEmail,
			freeRealtorStruct, freeEmail, compareMapKey);
	Map customers_map = mapCreate(copyCustomerStruct, copyEmail,
			freeCustomerStruct, freeEmail, compareMapKey);
	service->realtorsService = realtors_map;
	service->customersService = customers_map;
	return service;
}

void yad3ServiceDestroy(Yad3Service service) {
	mapDestroy(service->realtorsService);
	mapDestroy(service->customersService);
	free(service);
}

Yad3Result yad3RealtorAdd(Yad3Service service, char* email, char* company_name,
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
		free(new_realtor);
		mapDestroy(new_map);
		return YAD3_INVALID_PARAMETERS;
	}

	new_realtor->companyName = stringDuplicate(company_name);
	new_realtor->taxPercentage = tax_percentage;
	new_realtor->apartmentServices = new_map;
	new_realtor->rank = 0;

	mapPut(service->realtorsService, email, new_realtor);

	mapDestroy(new_map);
	free(new_realtor->companyName);
	free(new_realtor);
	return YAD3_SUCCESS;
}

Yad3Result yad3RealtorRemove(Yad3Service service, char* realtor_email) {
	if (service == NULL) {
		return YAD3_OUT_OF_MEMORY;	//Maybe define NULL_ARG instead
	}
	if (!validateEmail(realtor_email)) {
		return YAD3_INVALID_PARAMETERS;
	}
	bool customer_exists = mapContains(service->customersService,
			realtor_email);
	bool realtor_exists = mapContains(service->realtorsService, realtor_email);
	if (!realtor_exists) {
		return YAD3_EMAIL_DOES_NOT_EXIST;
	}
	if (customer_exists) {
		return YAD3_EMAIL_WRONG_ACCOUNT_TYPE;
	}
	mapRemove(service->realtorsService, realtor_email);
	return YAD3_SUCCESS;
}

Yad3Result yad3RealtorAddApartmentService(Yad3Service service, char* realtor_email,
		char* service_name, int max_size) {
//	assert(realtor_email && service_name && max_size);

	if (emailAlreadyExist(service, realtor_email, REALTOR) == false) {
		return YAD3_EMAIL_DOES_NOT_EXIST;
	}
	Realtor realtor = mapGet(service->realtorsService, realtor_email);
	if (apartmentServiceAlreadyExist(realtor->apartmentServices,
			service_name) == true) {
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
	Realtor tmp = (Realtor) realtor;
	Map current_map = tmp->apartmentServices;
	char* new_service_name = stringDuplicate(service_name);
	mapPut(current_map, new_service_name, apartment_service);
	free(new_service_name);
	serviceDestroy(apartment_service);

	return YAD3_SUCCESS;
}

Yad3Result yad3RealtorRemoveApartmentService(Yad3Service service, char* realtor_email,
		char* service_name) {
	if (service == NULL) {
		return YAD3_OUT_OF_MEMORY;
	}
	Realtor realtor = mapGet(service->realtorsService, (char*) realtor_email);
	if (realtor == NULL) {
		return YAD3_EMAIL_DOES_NOT_EXIST;
	}
	if (apartmentServiceAlreadyExist(realtor->apartmentServices,
			service_name) == false) {
		return YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST;
	}

	if (!validateEmail(realtor_email)) {
		return YAD3_INVALID_PARAMETERS;
	}
	MapResult result = mapRemove(realtor->apartmentServices,
			(char*) service_name);
	if (result == MAP_ITEM_DOES_NOT_EXIST) {
		return YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST;
	}
	return YAD3_SUCCESS;
}

Yad3Result yad3RealtorAddApartment(Yad3Service service, char* realtor_email,
		char* service_name, int id, int price, int width, int height,
		char* matrix) {
	if (service == NULL) {
		return YAD3_OUT_OF_MEMORY;
	}
	if (validateEmail(realtor_email) == false) { // ADD CHECK ABOUT ID, PRICE, WIDTH AND HEIGHT
		return YAD3_INVALID_PARAMETERS;
	}
	if (emailAlreadyExist(service, realtor_email, REALTOR) == false) {
		return YAD3_EMAIL_DOES_NOT_EXIST;
	}

	Map realtor_map = service->realtorsService;
	Realtor current_realtor = (Realtor) mapGet(realtor_map, realtor_email);
	Map all_apartment_services = current_realtor->apartmentServices;
	if (apartmentServiceAlreadyExist(all_apartment_services,
			service_name) == false) {
		return YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST;
		// free mem ?
	}
	ApartmentService apartment_service = (ApartmentService) mapGet(
			all_apartment_services, service_name);

	SquareType** matrix_grid = createMatrixGrid(matrix, height, width);
	Apartment new_apartment = apartmentCreate(matrix_grid, height, width,
			price);
	destroyMatrixGrid(matrix_grid, height);
	ApartmentServiceResult result = serviceAddApartment(apartment_service,
			new_apartment, id);
	apartmentDestroy(new_apartment);
	if (result == APARTMENT_SERVICE_FULL) {
		return YAD3_APARTMENT_SERVICE_FULL;
		//free mem
	}
	if (result == APARTMENT_SERVICE_ALREADY_EXISTS) {
		return YAD3_APARTMENT_ALREADY_EXISTS;
	}
	//printApartment(service, email, service_name, id); // REMOVE BEFORE SUBMITION

	return YAD3_SUCCESS;
}

Yad3Result yad3RealtorRemoveApartment(Yad3Service service, char* realtor_email,
		char* service_name, int id) {
	Map realtor_map = service->realtorsService;
	Realtor current_realtor = (Realtor) mapGet(realtor_map, realtor_email);
	Map all_apartment_services = current_realtor->apartmentServices;
	ApartmentService apartment_service = (ApartmentService) mapGet(
			all_apartment_services, service_name);
	ApartmentServiceResult result = serviceDeleteById(apartment_service, id);
	if (result == APARTMENT_SERVICE_NO_FIT) {
		return YAD3_APARTMENT_DOES_NOT_EXIST;
	}
	return YAD3_SUCCESS;
}

/* TBC */

Yad3Result yad3SignificantRealtors(Yad3Service service, int count) {
	if (count < 1) {
		return YAD3_INVALID_PARAMETERS;
	}
	Map realtors = service->realtorsService;
	Realtor realtor;
//	printRealtorData(realtors);
	int size = mapGetSize(realtors);
	long long int* rank_array = malloc(size * sizeof(rank_array));
	char** email_array = malloc(size * sizeof(*email_array));
	if (rank_array == NULL || email_array == NULL) {
		free(rank_array);
		arrayDestroy(email_array, size);
		return YAD3_OUT_OF_MEMORY;
	}
	MAP_FOREACH(MapKeyElement, email, realtors)
	{
		realtor = mapGet(realtors, email);
		realtor->rank = calculateRealtorRank(realtor);
//		printf("Rank of realtor with email %s is %lld\n", email, realtor->rank);
	}
	convertMapToArrays(realtors, rank_array, email_array);
	bubbleSort(rank_array, email_array, size);
	for (int i = 0; i < size && i < count; i++) {
		realtor = mapGet(realtors, email_array[i]);
		if (realtor->rank < 1) {
			count++;
		} else {
			mtmPrintRealtor(stdout, email_array[i], realtor->companyName);
		}
	}
	free(rank_array);
	arrayDestroy(email_array, size);
	return YAD3_SUCCESS;
}

Yad3Result yad3CustomerAdd(Yad3Service service, char* customer_email,
		int min_area, int min_rooms, int max_price) {
	if (emailAlreadyExist(service, customer_email, CUSTOMER) == true) {
		return YAD3_EMAIL_ALREADY_EXISTS;
	}
	if (min_area < 1 || min_rooms < 1 || max_price < 1
			|| !validateEmail(customer_email)) {
		return YAD3_INVALID_PARAMETERS;
	}
	Customer customer = malloc(sizeof(*customer));
	Set offers = setCreate(copyOffer, freeOffer, compareOffers);
	if (customer == NULL || offers == NULL) {
		free(customer);
		setDestroy(offers);
		return YAD3_OUT_OF_MEMORY;
	}
	customer->min_area = min_area;
	customer->min_rooms = min_rooms;
	customer->max_price = max_price;
	customer->money_spent = 0;
	customer->customerOffers = offers;
	return YAD3_SUCCESS;
}

Yad3Result yad3CustomerRemove(Yad3Service service, char* customer_email) {
	if (service == NULL) {
		return YAD3_OUT_OF_MEMORY;	//Maybe define NULL_ARG instead
	}
	if (!validateEmail(customer_email)) {
		return YAD3_INVALID_PARAMETERS;
	}
	bool customer_exists = mapContains(service->customersService,
			customer_email);
	bool realtor_exists = mapContains(service->realtorsService, customer_email);
	if (!customer_exists) {
		return YAD3_EMAIL_DOES_NOT_EXIST;
	}
	if (realtor_exists) {
		return YAD3_EMAIL_WRONG_ACCOUNT_TYPE;
	}
	mapRemove(service->customersService, customer_email);
	return YAD3_SUCCESS;
}

Yad3Result yad3CustomerPurchase(Yad3Service service, char* customer_email,
		char* realtor_email, char* service_name, int apartment_id) {
	if (service == NULL) {
		return YAD3_OUT_OF_MEMORY;	//Maybe define NULL_ARG instead
	}
	if (!validateEmail(customer_email) || !validateEmail(realtor_email)
			|| apartment_id < 0) {
		return YAD3_INVALID_PARAMETERS;
	}
	Yad3Result email_check = checkEmailExists(service, customer_email,
			realtor_email);
	if (email_check != YAD3_SUCCESS) {
		return email_check;
	}
	Realtor realtor = mapGet(service->realtorsService, realtor_email);
	Apartment apartment;
	Yad3Result apartment_check = checkRealtorHasApartment(realtor,
			customer_email, realtor_email, service_name, apartment_id,
			&apartment);
	if (apartment_check != YAD3_SUCCESS) {
		return apartment_check;
	}
	Customer customer = mapGet(service->customersService, customer_email);
	if (!apartmentParametersMatch(apartment, customer)) {
		return YAD3_PURCHASE_WRONG_PROPERTIES;
	}
	int final_price = calculateFinalPrice(apartment, realtor->taxPercentage);
	yad3RealtorRemoveApartment(service, realtor_email, service_name, apartment_id);
	customer->money_spent += final_price;
	removeAllOffersOnApartment(service, customer_email, realtor_email,
			service_name, apartment_id);   //IF ANY OFFERS EXIST - DELETE THEM
	return YAD3_SUCCESS;
}

Yad3Result yad3CustomerMakeOffer(Yad3Service service, char* customer_email,
		char* realtor_email, char* service_name, int apartment_id,
		int new_price) {
	if (service == NULL) {
		return YAD3_OUT_OF_MEMORY;	//Maybe define NULL_ARG instead
	}
	if (!validateEmail(customer_email) || !validateEmail(realtor_email)
			|| apartment_id < 0 || new_price < 0) {
		return YAD3_INVALID_PARAMETERS;
	}
	Yad3Result email_check = checkEmailExists(service, customer_email,
			realtor_email);
	if (email_check != YAD3_SUCCESS) {
		return email_check;
	}
	Realtor realtor = mapGet(service->realtorsService, realtor_email);
	Apartment apartment;
	Yad3Result apartment_check = checkRealtorHasApartment(realtor,
			customer_email, realtor_email, service_name, apartment_id,
			&apartment);
	if (apartment_check != YAD3_SUCCESS) {
		return apartment_check;
	}
	Customer customer = mapGet(service->customersService, customer_email);
	if (!apartmentParametersMatch(apartment, customer)) {
		return YAD3_REQUEST_WRONG_PROPERTIES;
	}
	int regular_price = calculateFinalPrice(apartment, realtor->taxPercentage);
	if (new_price <= regular_price) {
		return YAD3_REQUEST_ILLOGICAL_PRICE;
	}
	if (alreadySentOffer(customer, realtor_email)) {
		return YAD3_ALREADY_REQUESTED;
	}
	return createOffer(customer, realtor_email, service_name, apartment_id,
			new_price);
}

