#include <stdlib.h>
#include <uuid/uuid.h>

#include "gen_uuid.h"

char *gen_uuid() {
    uuid_t binuuid;
    uuid_generate_random(binuuid);
    char *uuid = (char *)malloc(37);
    uuid_unparse(binuuid, uuid);
    return uuid;
}