#include "Plant.h"

bool canPurchasePlant(int budget, int type) {
    return budget >= PLANT_PRICES[type];
}