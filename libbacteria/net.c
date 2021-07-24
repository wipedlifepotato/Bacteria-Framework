#include "net.h"

void set_timeout(int socket, unsigned int tSec, unsigned int tUsec) {
  struct timeval tv;
  tv.tv_sec = tSec;
  tv.tv_usec = tUsec;
  setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
}

char **split_msg(char *buf, const char schar, size_t *splitted_size,
                 size_t msg_len) {
  /*
   */
  const unsigned long long max_splitted = 120;
  size_t arr_size = 0;
  char **splitted;
  splitted = (char **)malloc(sizeof(char *) * arr_size);
  if (!splitted)
    abort();
  char *str;
  char str2[msg_len];
  do {
    bzero(str2, msg_len);
    str = strchr(buf, schar);

    if (str != NULL || (str = strchr(buf, '\r')) != NULL) {
      arr_size++;
      splitted = (char **)realloc(splitted, sizeof(char *) * arr_size);
      if (!splitted)
        abort();

      memcpy(str2, buf, (str - buf));
      // printf("str2 = %s\n", str2);

      splitted[arr_size - 1] = (char *)malloc(sizeof(char) * strlen(str2) + 1);
      if (!splitted[arr_size - 1])
        abort();

      strcpy(splitted[arr_size - 1], str2);
      splitted[arr_size - 1][strlen(str2)] = 0;
    }
    buf = str + 1;
  } while (str != NULL && arr_size < max_splitted);
  *splitted_size = arr_size;

  return splitted;
}

void free_splitted(char **what, size_t n) {
  for (n; n--;) {
    if (n == 0)
      break;
    if (what[n][0] == 0)
      continue;
    free((void *)(what[n - 1]));
  }
  free((void *)what);
}

char *packData(char **dataKeys, char **dataValues, char fsplitter,
               char ssplitter) {

  char **pKeys = dataKeys;
  char **pValues = dataValues;

  if (dataKeys[0] == NULL || dataValues[0] == NULL)
    return NULL;

  size_t sRet = 0; //
  char *rVal = malloc(sizeof(char) * 2);
  if (rVal == NULL)
    return NULL;
  for (unsigned long i = 0; pKeys[i] != NULL && pValues[i] != NULL; i++) {
    // strcat(sRet,dataKeys[i]);

    const size_t KeySize = strlen(dataKeys[i]);
    const size_t ValSize = strlen(dataValues[i]);
    const size_t fullSize = KeySize + ValSize + 2;
    rVal = realloc(rVal, (sRet + fullSize) * sizeof(char));

    strcpy(rVal + sRet, dataKeys[i]);

    rVal[KeySize + sRet] = fsplitter;

    strcpy(rVal + KeySize + 1 + sRet, dataValues[i]);

    rVal[KeySize + 1 + ValSize + sRet] = ssplitter;

    sRet += fullSize;
  }
  sRet++;
  rVal = realloc(rVal, (sRet) * sizeof(char));
  rVal[sRet] = 0;
  return (char *)rVal;
}

// fsplitter = '='; ssplitter=';' will be by default

char **unpackData(char *data, size_t *rt_size, char fsplitter, char ssplitter) {
  void *dFirst = data;
  size_t count_sSplitter = 0;
  size_t count_fSplitter = 0;
  size_t sData = 0;
  while (*data) {
    if (*(data) == fsplitter)
      count_fSplitter++;
    else if (*data == ssplitter)
      count_sSplitter++;
    data++;
    sData++;
  }

  if (count_sSplitter != count_fSplitter)
    return NULL; // TODO: maybe another way.
  data = (char *)dFirst;
  size_t f_split_size, s_split_size;

  char **ssplitted = split_msg(data, ssplitter, &s_split_size, sData);
  char **rVal = malloc(sizeof(char **) * (*rt_size));
  unsigned int z = 0;
  *rt_size = s_split_size * 2;
  for (unsigned int i = s_split_size; i--;) {
    char *fs = strchr(ssplitted[i], fsplitter);
    char *ptrFirst = ssplitted[i];
    char *ptrLast = &ssplitted[i][strlen(ssplitted[i])];
    if (fs == NULL) {
      rVal[z] = (char *)malloc(sizeof(char));
      rVal[z][0] = IGNOREINFOBYTE;
      rVal[z + 1] = (char *)malloc(sizeof(char));
      rVal[z + 1][0] = IGNOREINFOBYTE;
      z += 2;
      continue;
    }
    size_t s1 = (fs - ptrFirst), s2 = (ptrLast - fs);

    rVal[z] = malloc(sizeof(char) * s1 + 1);
    rVal[z + 1] = malloc(sizeof(char) * s2 + 1);
    bzero(rVal[z], s1 + 1);
    bzero(rVal[z + 1], s2 + 1);
    memcpy(rVal[z], ptrFirst, s1);
    memcpy(rVal[z + 1], fs + 1, s2);
    //		rVal[z][( ptrFirst - fs )+1] = '\0';
    //		rVal[z+1][( ptrLast - fs )+1] = '\0';
    // printf("rval; %s = %s\n", rVal[z], rVal[z+1]);
    z += 2;
  }

  free_splitted(ssplitted, s_split_size);
  return rVal;
}

char *join_data(const char *a, const char *b, const char split_char) {
  size_t sA = strlen(a);
  size_t sB = strlen(b);
  //	size_t lSize = 0;
  char *ret = malloc(sizeof(char) * (sA + sB) + 3); // ';' + ';' + \0
  memcpy(ret, a, sA);
  //	lSize += sA;
  ret[sA] = split_char;
  //	lSize++;
  memcpy((ret + sA + 1), b, sB); // plus ';'
                                 //	lSize+=sB;
  ret[sA + sB + 1] = split_char;
  //	lSize++;
  ret[sA + sB + 2] = '\0'; // plus ';' + ';'
  return ret;
}

char *join_addresses(const char *addr, ...) {

  va_list ap;
  va_start(ap, addr);
  size_t cSize = strlen(addr);
  char *pRet =
      malloc(cSize + 1 * sizeof(char)); // sizeof of char is 1 on must of OS
  // void * fRet = pRet;
  // bzero(pRet, cSize);
  strcpy(pRet, addr);
  pRet[cSize] = SPLITADDRCHAR;

  addr = va_arg(ap, char *);

  while (addr != NULL) {
    size_t addr_size = strlen(addr) + 1;
    pRet = (char *)realloc(pRet, (addr_size + cSize + 1) * sizeof(char));
    memcpy(pRet + cSize + 1, addr, addr_size);
    cSize += addr_size;
    pRet[cSize] = SPLITADDRCHAR;
    addr = va_arg(ap, char *);
  }

  pRet = realloc(pRet, cSize + 1);
  pRet[cSize] = '\0';
  return pRet;
}
