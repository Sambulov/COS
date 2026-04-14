#ifndef HDL_TRANSPORT_H_
#define HDL_TRANSPORT_H_

#define HDL_TRANSPORT_STREAM_LENGTH         0xffffffff

typedef enum {
  HDL_TRANSPORT_OPEN              = 0x01, /**< Begin listening (server mode) or try to connect(client mode). */
  HDL_TRANSPORT_ESTABLISHED       = 0x02, /**< New connection accepted (server mode) or connection success(client mode). */
  HDL_TRANSPORT_CLOSED            = 0x04, /**< Transport broken; resources should be freed. */
  HDL_TRANSPORT_TRANSFER_BUFER    = 0x10, /**< Outgoing buffer provided for filling. */
  HDL_TRANSPORT_TRANSFER_COMPLETE = 0x20, /**< Send transaction complete. */
  HDL_TRANSPORT_TRANSFER_ERR      = 0x40, /**< Last send operation failed or terminated. */
  HDL_TRANSPORT_RECEIVED          = 0x80, /**< Data received. */
} hdl_transport_event_type_t;

typedef enum {
  HDL_TRANSPORT_INVALID           = 0x00,
  HDL_TRANSPORT_OK                = 0x01, /**< Call processed. */
  HDL_TRANSPORT_BUSY              = 0x02, /**< Transport is busy. Retry later */
  HDL_TRANSPORT_BAD_CALL          = 0x04, /**< There are bad arguments provided or unsupported */
  HDL_TRANSPORT_FAULT             = 0x08, /**< Internal error */
} hdl_transport_result_t;

/**
 * @brief Event data structure passed to the event handler.
 *
 * The union distinguishes between receive events (rx) and ready to send events (tx).
 * The structure is valid only during the callback; data must be copied if needed later.
 */
typedef struct {
  hdl_transport_event_type_t event; /**< Event type (one of the status flags). */
  union {
    struct {
      void *ptr;        /**< Received data (read-only). */
      uint32_t length;  /**< Number of bytes in this fragment. */
      uint32_t left;    /**< Remaining bytes in the current message (0 if complete, HDL_TRANSPORT_STREAM_LENGTH - unknown). */
    } data;             /**< Valid for HDL_TRANSPORT_RECEIVED event. */
    struct {
      void *ptr;        /**< Buffer provided by transport to be filled. */
      uint32_t size;    /**< Size of the provided buffer. */
      uint32_t fill;    /**< Fragment writed size, set by user (set 0 => send stretching). */
    } buffer;           /**< Valid for HDL_TRANSPORT_TRANSFER events. */
  };
} hdl_transport_event_trigger_t;

typedef hdl_transport_result_t (*hdl_transport_open_t)(const void *self, const void *config);
typedef hdl_transport_result_t (*hdl_transport_op_t)(const void *self);
typedef hdl_transport_result_t (*hdl_transport_begin_send_t)(const void *self, void *data, uint32_t length);

typedef struct {
  hdl_module_initializer_t init;
  hdl_transport_open_t open;
  hdl_transport_op_t close;
  hdl_transport_begin_send_t begin_send;
  hdl_transport_op_t break_sending;
  hdl_event_subscribtion_t subscribe;
} hdl_transport_iface_t;

hdl_module_new_t(hdl_transport_t, 0, void *, hdl_transport_iface_t);

/**
 * @brief Open the transport.
 * @param self    Transport instance.
 * @param config  Configuration (implementation‑specific), NULL if default.
 * @return        One of: HDL_TRANSPORT_CALL_OK, HDL_TRANSPORT_CALL_BUSY,
 *                HDL_TRANSPORT_BAD_CALL, or HDL_TRANSPORT_INVALID.
 */
__STATIC_INLINE hdl_transport_result_t hdl_transport_open(const void *self, const void *config) {
  MODULE_ASSERT(self, HDL_TRANSPORT_INVALID);
  return ((hdl_transport_iface_t *)((hdl_module_base_t *)self)->iface)->open(self, config);
}

/**
 * @brief Close the transport.
 * @param self  Transport instance.
 * @return        One of: HDL_TRANSPORT_CALL_OK, HDL_TRANSPORT_CALL_BUSY,
 *                HDL_TRANSPORT_BAD_CALL, or HDL_TRANSPORT_INVALID.
 */
__STATIC_INLINE hdl_transport_result_t hdl_transport_close(const void *self) {
  MODULE_ASSERT(self, HDL_TRANSPORT_INVALID);
  return ((hdl_transport_iface_t *)((hdl_module_base_t *)self)->iface)->close(self);
}

/**
 * @brief Send data using zero‑copy.
 * The provided buffer is used directly by the transport. The buffer must stay valid
 * until the transaction completes.
 * @param self    Transport instance.
 * @param data    Data buffer.
 * @param length  Number of bytes to send.
 * @return        One of: HDL_TRANSPORT_CALL_OK, HDL_TRANSPORT_CALL_BUSY,
 *                HDL_TRANSPORT_BAD_CALL, or HDL_TRANSPORT_INVALID.
 */
__STATIC_INLINE hdl_transport_result_t hdl_transport_send_zero_copy(const void *self, void *data, uint32_t length) {
  MODULE_ASSERT(self, HDL_TRANSPORT_INVALID);
  return ((hdl_transport_iface_t *)((hdl_module_base_t *)self)->iface)->begin_send(self, data, length);
}

/**
 * @brief Request a buffer of fixed size for later filling.
 * The transport will later generate a HDL_TRANSPORT_BUFER event with a buffer 
 * The caller must be ready to copy data into that buffer.
 * There are may be multiple events if the transport cannot provide a full-size buffer. 
 * The caller must be ready to provide the data in parts.
 * @param self             Transport instance.
 * @param expected_length  Transaction length.
 * @return        One of: HDL_TRANSPORT_CALL_OK, HDL_TRANSPORT_CALL_BUSY,
 *                HDL_TRANSPORT_BAD_CALL, or HDL_TRANSPORT_INVALID.
 */
__STATIC_INLINE hdl_transport_result_t hdl_transport_send(const void *self, uint32_t expected_length) {
  MODULE_ASSERT(self, HDL_TRANSPORT_INVALID);
  return ((hdl_transport_iface_t *)((hdl_module_base_t *)self)->iface)->begin_send(self, NULL, expected_length);
}

/**
 * @brief Start a streaming send operation.
 * The transport will provide buffers of arbitrary size via HDL_TRANSPORT_BUFER events
 * until the stream is ended (e.g., by calling break_sending).
 * @param self  Transport instance.
 * @return        One of: HDL_TRANSPORT_CALL_OK, HDL_TRANSPORT_CALL_BUSY,
 *                HDL_TRANSPORT_BAD_CALL, or HDL_TRANSPORT_INVALID.
 */
__STATIC_INLINE hdl_transport_result_t hdl_transport_send_stream(const void *self) {
  MODULE_ASSERT(self, HDL_TRANSPORT_INVALID);
  return ((hdl_transport_iface_t *)((hdl_module_base_t *)self)->iface)->begin_send(self, NULL, HDL_TRANSPORT_STREAM_LENGTH);
}

/**
 * @brief Abort the current send operation.
 * Cancels any ongoing transmission. May generate a DATA_SENDING_ERR event.
 * @param self  Transport instance.
 * @return        One of: HDL_TRANSPORT_CALL_OK, HDL_TRANSPORT_CALL_BUSY,
 *                HDL_TRANSPORT_INVALID.
 */
__STATIC_INLINE hdl_transport_result_t hdl_transport_break_sending(const void *self) {
  MODULE_ASSERT(self, HDL_TRANSPORT_INVALID);
  return ((hdl_transport_iface_t *)((hdl_module_base_t *)self)->iface)->break_sending(self);
}

/**
 * @brief Subscribe to events on a transport.
 * @param desc      Transport instance.
 * @param delegate  Delegate to add.
 */
__STATIC_INLINE void hdl_transport_subscribe(const void *desc, hdl_delegate_t *delegate) {
  MODULE_ASSERT(desc, );
  return ((hdl_transport_iface_t *)((hdl_module_base_t *)desc)->iface)->subscribe(desc, delegate);
}

#endif /* HDL_TRANSPORT_H_ */
