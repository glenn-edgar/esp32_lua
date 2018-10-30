bool analyize_current_object(cmp_ctx_t *ctx, cmp_object_t *obj,uint32_t *skip_number, uint32_t *size) 
{
  uint8_t type_marker = 0;
  *size = 0;
  if (!read_type_marker(ctx, &type_marker))
    return false;
  //printf("type marker is %x number to skip %d size %d\n",type_marker,*skip_number,  *size);
  if (type_marker <= 0x7F) {
    obj->type = CMP_TYPE_POSITIVE_FIXNUM;
    obj->as.u8 = type_marker;
  }
  else if (type_marker <= 0x8F) {
    obj->type = CMP_TYPE_FIXMAP;
    obj->as.map_size = type_marker & FIXMAP_SIZE;
    *size = 0;
    *skip_number += obj->as.map_size*2;
  }
  else if (type_marker <= 0x9F) {
    obj->type = CMP_TYPE_FIXARRAY;
    obj->as.array_size = type_marker & FIXARRAY_SIZE;
    *size = 0;
    *skip_number +=  obj->as.array_size;
  }
  else if (type_marker <= 0xBF) {
    obj->type = CMP_TYPE_FIXSTR;
    obj->as.str_size = type_marker & FIXSTR_SIZE;
    *size = obj->as.str_size;
  }
  else if (type_marker == NIL_MARKER) {
    obj->type = CMP_TYPE_NIL;
    obj->as.u8 = 0;
    
  }
  else if (type_marker == FALSE_MARKER) {
    obj->type = CMP_TYPE_BOOLEAN;
    obj->as.boolean = false;
  }
  else if (type_marker == TRUE_MARKER) {
    obj->type = CMP_TYPE_BOOLEAN;
    obj->as.boolean = true;
  }
  else if (type_marker == BIN8_MARKER) {
    obj->type = CMP_TYPE_BIN8;
    if (!ctx->read(ctx, &obj->as.u8, sizeof(uint8_t))) {
      ctx->error = LENGTH_READING_ERROR;
      return false;
    }
    obj->as.bin_size = obj->as.u8;
    *size = obj->as.bin_size;
  }
  else if (type_marker == BIN16_MARKER) {
    obj->type = CMP_TYPE_BIN16;
    if (!ctx->read(ctx, &obj->as.u16, sizeof(uint16_t))) {
      ctx->error = LENGTH_READING_ERROR;
      return false;
    }
    obj->as.bin_size = be16(obj->as.u16);
    *size = obj->as.bin_size;
  }
  else if (type_marker == BIN32_MARKER) {
    obj->type = CMP_TYPE_BIN32;
    if (!ctx->read(ctx, &obj->as.u32, sizeof(uint32_t))) {
      ctx->error = LENGTH_READING_ERROR;
      return false;
    }
    obj->as.bin_size = be32(obj->as.u32);
    *size = obj->as.bin_size;
  }
  else if (type_marker == EXT8_MARKER) {
    uint8_t ext_size;
    int8_t ext_type;

    obj->type = CMP_TYPE_EXT8;
    if (!ctx->read(ctx, &ext_size, sizeof(uint8_t))) {
      ctx->error = LENGTH_READING_ERROR;
      return false;
    }
    if (!ctx->read(ctx, &ext_type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = ext_size;
    obj->as.ext.type = ext_type;
    *size = obj->as.ext.size;
  }
  else if (type_marker == EXT16_MARKER) {
    int8_t ext_type;
    uint16_t ext_size;

    obj->type = CMP_TYPE_EXT16;
    if (!ctx->read(ctx, &ext_size, sizeof(uint16_t))) {
      ctx->error = LENGTH_READING_ERROR;
      return false;
    }
    if (!ctx->read(ctx, &ext_type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = be16(ext_size);
    obj->as.ext.type = ext_type;
    *size = obj->as.ext.size;
  }
  else if (type_marker == EXT32_MARKER) {
    int8_t ext_type;
    uint32_t ext_size;

    obj->type = CMP_TYPE_EXT32;
    if (!ctx->read(ctx, &ext_size, sizeof(uint32_t))) {
      ctx->error = LENGTH_READING_ERROR;
      return false;
    }
    if (!ctx->read(ctx, &ext_type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = be32(ext_size);
    obj->as.ext.type = ext_type;
    *size = obj->as.ext.size;
  }
  else if (type_marker == FLOAT_MARKER) {
    char bytes[sizeof(float)];

    obj->type = CMP_TYPE_FLOAT;
    if (!ctx->read(ctx, bytes, sizeof(float))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.flt = decode_befloat(bytes);
  }
  else if (type_marker == DOUBLE_MARKER) {
    char bytes[sizeof(double)];

    obj->type = CMP_TYPE_DOUBLE;
    if (!ctx->read(ctx, bytes, sizeof(double))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.dbl = decode_bedouble(bytes);
  }
  else if (type_marker == U8_MARKER) {
    obj->type = CMP_TYPE_UINT8;
    if (!ctx->read(ctx, &obj->as.u8, sizeof(uint8_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
  }
  else if (type_marker == U16_MARKER) {
    obj->type = CMP_TYPE_UINT16;
    if (!ctx->read(ctx, &obj->as.u16, sizeof(uint16_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.u16 = be16(obj->as.u16);
  }
  else if (type_marker == U32_MARKER) {
    obj->type = CMP_TYPE_UINT32;
    if (!ctx->read(ctx, &obj->as.u32, sizeof(uint32_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.u32 = be32(obj->as.u32);
  }
  else if (type_marker == U64_MARKER) {
    obj->type = CMP_TYPE_UINT64;
    if (!ctx->read(ctx, &obj->as.u64, sizeof(uint64_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.u64 = be64(obj->as.u64);
  }
  else if (type_marker == S8_MARKER) {
    obj->type = CMP_TYPE_SINT8;
    if (!ctx->read(ctx, &obj->as.s8, sizeof(int8_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
  }
  else if (type_marker == S16_MARKER) {
    obj->type = CMP_TYPE_SINT16;
    if (!ctx->read(ctx, &obj->as.s16, sizeof(int16_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.s16 = be16(obj->as.s16);
  }
  else if (type_marker == S32_MARKER) {
    obj->type = CMP_TYPE_SINT32;
    if (!ctx->read(ctx, &obj->as.s32, sizeof(int32_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.s32 = be32(obj->as.s32);
  }
  else if (type_marker == S64_MARKER) {
    obj->type = CMP_TYPE_SINT64;
    if (!ctx->read(ctx, &obj->as.s64, sizeof(int64_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.s64 = be64(obj->as.s64);
  }
  else if (type_marker == FIXEXT1_MARKER) {
    obj->type = CMP_TYPE_FIXEXT1;
    if (!ctx->read(ctx, &obj->as.ext.type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = 1;
  }
  else if (type_marker == FIXEXT2_MARKER) {
    obj->type = CMP_TYPE_FIXEXT2;
    if (!ctx->read(ctx, &obj->as.ext.type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = 2;
    *size = 2;
  }
  else if (type_marker == FIXEXT4_MARKER) {
    obj->type = CMP_TYPE_FIXEXT4;
    if (!ctx->read(ctx, &obj->as.ext.type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = 4;
    *size = 4;
  }
  else if (type_marker == FIXEXT8_MARKER) {
    obj->type = CMP_TYPE_FIXEXT8;
    if (!ctx->read(ctx, &obj->as.ext.type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = 8;
    *size = 8;
  }
  else if (type_marker == FIXEXT16_MARKER) {
    obj->type = CMP_TYPE_FIXEXT16;
    if (!ctx->read(ctx, &obj->as.ext.type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = 16;
    *size = 16;
  }
  else if (type_marker == STR8_MARKER) {
    obj->type = CMP_TYPE_STR8;
    if (!ctx->read(ctx, &obj->as.u8, sizeof(uint8_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.str_size = obj->as.u8;
    *size = obj->as.u8;
  }
  else if (type_marker == STR16_MARKER) {
    obj->type = CMP_TYPE_STR16;
    if (!ctx->read(ctx, &obj->as.u16, sizeof(uint16_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.str_size = be16(obj->as.u16);
    *size = obj->as.str_size;
  }
  else if (type_marker == STR32_MARKER) {
    obj->type = CMP_TYPE_STR32;
    if (!ctx->read(ctx, &obj->as.u32, sizeof(uint32_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.str_size = be32(obj->as.u32);
    *size = obj->as.str_size;
  }
  else if (type_marker == ARRAY16_MARKER) {
    obj->type = CMP_TYPE_ARRAY16;
    if (!ctx->read(ctx, &obj->as.u16, sizeof(uint16_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.array_size = be16(obj->as.u16);
    *size =0;
    *skip_number += obj->as.array_size;
  }
  else if (type_marker == ARRAY32_MARKER) {
    obj->type = CMP_TYPE_ARRAY32;
    if (!ctx->read(ctx, &obj->as.u32, sizeof(uint32_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.array_size = be32(obj->as.u32);
    *size = 0;
    *skip_number += obj->as.array_size;
  }
  else if (type_marker == MAP16_MARKER) {
    obj->type = CMP_TYPE_MAP16;
    if (!ctx->read(ctx, &obj->as.u16, sizeof(uint16_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.map_size = be16(obj->as.u16);
    *size =0;
    *skip_number += obj->as.map_size*2;
  }
  else if (type_marker == MAP32_MARKER) {
    obj->type = CMP_TYPE_MAP32;
    if (!ctx->read(ctx, &obj->as.u32, sizeof(uint32_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.map_size = be32(obj->as.u32);
    *size =0;
    *skip_number += obj->as.map_size*2;
  }
  else if (type_marker >= NEGATIVE_FIXNUM_MARKER) {
    obj->type = CMP_TYPE_NEGATIVE_FIXNUM;
    obj->as.s8 = type_marker;
  }
  else {
    ctx->error = INVALID_TYPE_ERROR;
    return false;
  }
  //printf("type marker is %x number to skip %d size %d\n",type_marker,*skip_number,  *size);
  return true;
}
