#ifndef YAD3SERVICE_H_
#define YAD3SERVICE_H_

#include "map.h"
#include "set.h"
#include "apartment_service.h"

/** Type used for returning error codes from yad3 functions */
typedef enum {
	YAD3_SUCCESS,
	YAD3_OUT_OF_MEMORY,
	YAD3_INVALID_PARAMETERS,
	YAD3_EMAIL_ALREADY_EXISTS,
	YAD3_EMAIL_DOES_NOT_EXIST,
	YAD3_EMAIL_WRONG_ACCOUNT_TYPE,
	YAD3_ALREADY_REQUESTED,
	YAD3_NOT_REQUESTED,
	YAD3_APARTMENT_SERVICE_ALREADY_EXISTS,
	YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST,
	YAD3_APARTMENT_SERVICE_FULL,
	YAD3_APARTMENT_ALREADY_EXISTS,
	YAD3_APARTMENT_DOES_NOT_EXIST,
	YAD3_PURCHASE_WRONG_PROPERTIES,
	YAD3_REQUEST_WRONG_PROPERTIES,
	YAD3_REQUEST_ILLOGICAL_PRICE,
} Yad3Result;

typedef enum {
	DECLINE,
	ACCEPT,
	PENDING,
} Choice;

typedef enum {
	REALTOR,
	CUSTOMER,
} User;

typedef struct Yad3Service_t* Yad3Service;
typedef struct Realtor_t* Realtor;
typedef struct Customer_t* Customer;
typedef struct offer_t* Offer;

Yad3Service yad3ServiceCreate();
void yad3ServiceDestroy(Yad3Service service);

Yad3Result yad3AddRealtor(Yad3Service service, char* email, char* company_name,
		int tax_percentage);

Yad3Result yad3DeleteRealtor(Yad3Service service, char* key);

Yad3Result yad3AddApartmentService(Yad3Service service, char* realtor_email,
	char* service_name, int max_size);

Yad3Result yad3RemoveApartmentService(Yad3Service service, char* realtor_email,
	char* service_name);

Yad3Result yad3AddApartment(Map realtor_map, char* realtor_email,
	char* service_name, int id, Apartment apartment);

Yad3Result yad3RemoveApartment(Map realtor_map, char* realtor_email,
	char* service_name, int id);

Yad3Result yad3RespondToOffer(Map realtor_map, char* realtor_email,
	char* customer_email, Choice response);

Yad3Result yad3RelevantRealtors(Map realtor_map, char* customer_email);

Yad3Result yad3SignificantRealtors(Map realtor_map, int count);

Yad3Result yad3AddCustomer(Map customer_map, char* customer_email, int min_area,
	int min_rooms, int max_price);

Yad3Result yad3RemoveCustomer(Map customer_map, char* customer_email);

Yad3Result yad3Purchase(Map customer_map,
		Map realtor_map, char* customer_email, char* realtor_email,
	char* service_name, int apartment_id);

Yad3Result yad3MakeOffer(Map customer_map,
		Map realtor_map, char* customer_email, char* realtor_email,
	char* service_name, int apartment_id, int new_price);

Yad3Result yad3MostPayingCustomers(Map customer_map, int count);

#endif /* YAD3SERVICE_H_ */
