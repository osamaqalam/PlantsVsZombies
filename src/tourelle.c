#include "tourelle.h"

bool canPurchaseTourelle(int cagnotte, int type) {
    return cagnotte >= TOURELLE_PRICES[type];
}