/*
 * Copyright 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * 3. Neither the name of copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"

#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"

#include "usb_device_composite.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Outgoing notification buffer number */
#define USB_CIC_VCOM_IN_ENDPOINT (1)
/* Outgoing data buffer number */
#define USB_DIC_VCOM_IN_ENDPOINT (2)
/* Outgoing data buffer index */
#define USB_DIC_VCOM_IN_ENDPOINT_INDEX (0)
/* Incoming data buffer number */
#define USB_DIC_VCOM_OUT_ENDPOINT (1)
/* Incoming data buffer index */
#define USB_DIC_VCOM_OUT_ENDPOINT_INDEX (1)
/* Data packet size for High Speed */
#define HS_DIC_VCOM_OUT_PACKET_SIZE (512)
/* Data packet size for Full Speed */
#define FS_DIC_VCOM_OUT_PACKET_SIZE (64)

#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0)
#define DATA_BUFF_SIZE HS_DIC_VCOM_OUT_PACKET_SIZE

#endif
#if defined(USB_DEVICE_CONFIG_KHCI) && (USB_DEVICE_CONFIG_KHCI > 0)
#define DATA_BUFF_SIZE FS_DIC_VCOM_OUT_PACKET_SIZE

#endif
#if defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U)
#define DATA_BUFF_SIZE FS_DIC_VCOM_OUT_PACKET_SIZE

#endif

#if defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U)
#define DATA_BUFF_SIZE HS_DIC_VCOM_OUT_PACKET_SIZE
#endif

/* Currently configured line coding */
#define LINE_CODING_SIZE (0x07U)
#define LINE_CODING_DTERATE (115200U)
#define LINE_CODING_CHARFORMAT (0x00U)
#define LINE_CODING_PARITYTYPE (0x00U)
#define LINE_CODING_DATABITS (0x08U)

/* Communications feature */
#define COMM_FEATURE_DATA_SIZE (0x02U)
#define STATUS_ABSTRACT_STATE (0x0000U)
#define COUNTRY_SETTING (0x0000U)

/* Notification of serial state */
#define NOTIF_PACKET_SIZE (0x08U)
#define UART_BITMAP_SIZE (0x02U)
#define NOTIF_REQUEST_TYPE (0xA1U)

/* Define the types for application */
typedef struct _usb_cdc_vcom_struct
{
    class_handle_t cdcAcmHandle; /* USB CDC ACM class handle.                                                         */
    volatile uint8_t startTransactions; /* A flag to indicate whether a CDC device is ready to transmit and receive data.    */
} usb_cdc_vcom_struct_t;

/* Define the information relates to abstract control model */
typedef struct _usb_cdc_acm_info
{
    uint8_t serialStateBuf[NOTIF_PACKET_SIZE + UART_BITMAP_SIZE]; /* Serial state buffer of the CDC device to notify the
                                                                     serial state to host. */
    bool dtePresent;          /* A flag to indicate whether DTE is present.         */
    uint16_t breakDuration;   /* Length of time in milliseconds of the break signal */
    uint8_t dteStatus;        /* Status of data terminal equipment                  */
    uint8_t currentInterface; /* Current interface index.                           */
    uint16_t uartState;       /* UART state of the CDC device.                      */
} usb_cdc_acm_info_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

usb_status_t USB_DeviceInterface0CicVcomCallback(class_handle_t handle, uint32_t event, void *param);

usb_status_t USB_DeviceInterface0CicVcomSetConfiguration(class_handle_t handle, uint8_t configuration_idx);

usb_status_t USB_DeviceInterface0CicVcomSetInterface(class_handle_t handle, uint8_t alternateSetting);

usb_status_t USB_DeviceInterface1DicVcomSetInterface(class_handle_t handle, uint8_t alternateSetting);

usb_status_t USB_DeviceInterface0CicVcomInit(usb_device_composite_struct_t *deviceComposite);

/*******************************************************************************
 * Variables
 ******************************************************************************/


extern usb_device_endpoint_struct_t g_UsbDeviceInterface1DicVcomSetting0DefaultEndpoints[];

/* Line coding of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_lineCoding[LINE_CODING_SIZE] = {
    /* E.g. 0x00,0xC2,0x01,0x00 : 0x0001C200 is 115200 bits per second */
    (LINE_CODING_DTERATE >> 0U) & 0x000000FFU,
    (LINE_CODING_DTERATE >> 8U) & 0x000000FFU,
    (LINE_CODING_DTERATE >> 16U) & 0x000000FFU,
    (LINE_CODING_DTERATE >> 24U) & 0x000000FFU,
    LINE_CODING_CHARFORMAT,
    LINE_CODING_PARITYTYPE,
    LINE_CODING_DATABITS};

/* Abstract state of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_abstractState[COMM_FEATURE_DATA_SIZE] = {(STATUS_ABSTRACT_STATE >> 0U) & 0x00FFU,
                                                          (STATUS_ABSTRACT_STATE >> 8U) & 0x00FFU};

/* Country code of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_countryCode[COMM_FEATURE_DATA_SIZE] = {(COUNTRY_SETTING >> 0U) & 0x00FFU,
                                                        (COUNTRY_SETTING >> 8U) & 0x00FFU};

/* CDC ACM information */
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static usb_cdc_acm_info_t s_usbCdcAcmInfo;
/* Data buffer for receiving and sending*/
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_currRecvBuf[DATA_BUFF_SIZE];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_currSendBuf[DATA_BUFF_SIZE];
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) volatile static uint32_t s_recvSize = 0;
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) volatile static uint32_t s_sendSize = 0;

#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
    defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
    defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) volatile static uint8_t s_waitForDataReceive = 0;
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) volatile static uint8_t s_comOpen = 0;
#endif

USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static usb_device_composite_struct_t *s_UsbDeviceComposite;
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static usb_cdc_vcom_struct_t s_UsbInterface0CicVcom;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Interface task function.
 */
void USB_DeviceInterface0CicVcomTask(void);

void USB_DeviceInterface0CicVcomTask();

void USB_read(uint8_t * buffer, uint8_t size)
{
	s_UsbInterface0CicVcom.startTransactions = 1;
	if (1 == s_UsbDeviceComposite->attach)
	{
		/* User Code */
		if (s_recvSize >= size)
		{
			s_recvSize = s_recvSize - size;

			for(int i = 0; i < size; i++)
			{
				buffer[i] = s_currRecvBuf[i];
			}
		}
	}

	USB_DeviceCdcAcmSend(s_UsbInterface0CicVcom.cdcAcmHandle, USB_DIC_VCOM_IN_ENDPOINT, NULL, 0);
}


void USB_write(uint8_t addres, float val)
{
	uint8_t array[5] = {};

	array[0] = addres;
	uint8_t * tmp = (uint8_t*)(&val);

	for(int i = 0; i < 4; i++){
		array[i+1] = tmp[i];
	}

	while(USB_DeviceCdcAcmSend(s_UsbInterface0CicVcom.cdcAcmHandle, USB_DIC_VCOM_IN_ENDPOINT, array, 5) == kStatus_USB_Busy);
}

/*!
* @brief Function that processes class specific events.
*
* @param handle Handle to USB device class.
* @param event Class event code.
* @param param	The parameter of the class specific event.
* @return usb_status_t Status of USB transaction.
*/
usb_status_t USB_DeviceInterface0CicVcomCallback(class_handle_t handle, uint32_t event, void *param)
{
    uint32_t len;
    uint8_t *uartBitmap;
    usb_device_cdc_acm_request_param_struct_t *acmReqParam;
    usb_device_endpoint_callback_message_struct_t *epCbParam;
    usb_status_t error = kStatus_USB_Error;
    usb_cdc_acm_info_t *acmInfo = &s_usbCdcAcmInfo;
    acmReqParam = (usb_device_cdc_acm_request_param_struct_t *)param;
    epCbParam = (usb_device_endpoint_callback_message_struct_t *)param;

    switch (event)
    {
        case kUSB_DeviceCdcEventSendResponse:
        {
            if ((epCbParam->length != 0) && (!(epCbParam->length % g_UsbDeviceInterface1DicVcomSetting0DefaultEndpoints[USB_DIC_VCOM_IN_ENDPOINT_INDEX].maxPacketSize)))
            {
                /* If the last packet is the size of endpoint, then send also zero-ended packet,
                 ** meaning that we want to inform the host that we do not have any additional
                 ** data, so it can flush the output.
                 */
                error = USB_DeviceCdcAcmSend(handle, USB_DIC_VCOM_IN_ENDPOINT, NULL, 0);
            }
            else if ((1 == s_UsbDeviceComposite->attach) && (1 == s_UsbInterface0CicVcom.startTransactions))
            {
                if ((epCbParam->buffer != NULL) || ((epCbParam->buffer == NULL) && (epCbParam->length == 0)))
                {
                    /* User: add your own code for send complete event */
                    /* Schedule buffer for next receive event */
                    error = USB_DeviceCdcAcmRecv(handle, USB_DIC_VCOM_OUT_ENDPOINT, s_currRecvBuf,
                                                 g_UsbDeviceInterface1DicVcomSetting0DefaultEndpoints[USB_DIC_VCOM_OUT_ENDPOINT_INDEX].maxPacketSize);
#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
    defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
    defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
                    s_waitForDataReceive = 1;
                    USB0->INTEN &= ~USB_INTEN_SOFTOKEN_MASK;
#endif
                }
            }
            else
            {
            }
        }
        break;
        case kUSB_DeviceCdcEventRecvResponse:
        {
            if ((1 == s_UsbDeviceComposite->attach) && (1 == s_UsbInterface0CicVcom.startTransactions))
            {
                s_recvSize = epCbParam->length;

#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
    defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
    defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
                s_waitForDataReceive = 0;
                USB0->INTEN |= USB_INTEN_SOFTOKEN_MASK;
#endif
                if (!s_recvSize)
                {
                    /* Schedule buffer for next receive event */
                    error = USB_DeviceCdcAcmRecv(handle, USB_DIC_VCOM_OUT_ENDPOINT, s_currRecvBuf,
                                                 g_UsbDeviceInterface1DicVcomSetting0DefaultEndpoints[USB_DIC_VCOM_OUT_ENDPOINT_INDEX].maxPacketSize);
#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
    defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
    defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
                    s_waitForDataReceive = 1;
                    USB0->INTEN &= ~USB_INTEN_SOFTOKEN_MASK;
#endif
                }
            }
        }
        break;
        case kUSB_DeviceCdcEventSerialStateNotif:
            ((usb_device_cdc_acm_struct_t *)handle)->hasSentState = 0;
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventSendEncapsulatedCommand:
            break;
        case kUSB_DeviceCdcEventGetEncapsulatedResponse:
            break;
        case kUSB_DeviceCdcEventSetCommFeature:
            if (USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE == acmReqParam->setupValue)
            {
                if (1 == acmReqParam->isSetup)
                {
                    *(acmReqParam->buffer) = s_abstractState;
                }
                else
                {
                    *(acmReqParam->length) = 0;
                }
            }
            else if (USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING == acmReqParam->setupValue)
            {
                if (1 == acmReqParam->isSetup)
                {
                    *(acmReqParam->buffer) = s_countryCode;
                }
                else
                {
                    *(acmReqParam->length) = 0;
                }
            }
            else
            {
            }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventGetCommFeature:
            if (USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE == acmReqParam->setupValue)
            {
                *(acmReqParam->buffer) = s_abstractState;
                *(acmReqParam->length) = COMM_FEATURE_DATA_SIZE;
            }
            else if (USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING == acmReqParam->setupValue)
            {
                *(acmReqParam->buffer) = s_countryCode;
                *(acmReqParam->length) = COMM_FEATURE_DATA_SIZE;
            }
            else
            {
            }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventClearCommFeature:
            break;
        case kUSB_DeviceCdcEventGetLineCoding:
            *(acmReqParam->buffer) = s_lineCoding;
            *(acmReqParam->length) = LINE_CODING_SIZE;
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventSetLineCoding:
            if (1 == acmReqParam->isSetup)
            {
                *(acmReqParam->buffer) = s_lineCoding;
            }
            else
            {
                *(acmReqParam->length) = 0;
            }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventSetControlLineState:
        {
            s_usbCdcAcmInfo.dteStatus = acmReqParam->setupValue;
            /* activate/deactivate Tx carrier */
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION)
            {
                acmInfo->uartState |= USB_DEVICE_CDC_UART_STATE_TX_CARRIER;
            }
            else
            {
                acmInfo->uartState &= (uint16_t)~USB_DEVICE_CDC_UART_STATE_TX_CARRIER;
            }

            /* activate carrier and DTE. Com port of terminal tool running on PC is open now */
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE)
            {
                acmInfo->uartState |= USB_DEVICE_CDC_UART_STATE_RX_CARRIER;
            }
            /* Com port of terminal tool running on PC is closed now */
            else
            {
                acmInfo->uartState &= (uint16_t)~USB_DEVICE_CDC_UART_STATE_RX_CARRIER;
            }

            /* Indicates to DCE if DTE is present or not */
            acmInfo->dtePresent = (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE) ? true : false;

            /* Initialize the serial state buffer */
            acmInfo->serialStateBuf[0] = NOTIF_REQUEST_TYPE;                /* bmRequestType */
            acmInfo->serialStateBuf[1] = USB_DEVICE_CDC_NOTIF_SERIAL_STATE; /* bNotification */
            acmInfo->serialStateBuf[2] = 0x00;                              /* wValue */
            acmInfo->serialStateBuf[3] = 0x00;
            acmInfo->serialStateBuf[4] = 0x00; /* wIndex */
            acmInfo->serialStateBuf[5] = 0x00;
            acmInfo->serialStateBuf[6] = UART_BITMAP_SIZE; /* wLength */
            acmInfo->serialStateBuf[7] = 0x00;
            /* Notify to host the line state */
            acmInfo->serialStateBuf[4] = acmReqParam->interfaceIndex;
            /* Lower byte of UART BITMAP */
            uartBitmap = (uint8_t *)&acmInfo->serialStateBuf[NOTIF_PACKET_SIZE + UART_BITMAP_SIZE - 2];
            uartBitmap[0] = acmInfo->uartState & 0xFFu;
            uartBitmap[1] = (acmInfo->uartState >> 8) & 0xFFu;
            len = (uint32_t)(NOTIF_PACKET_SIZE + UART_BITMAP_SIZE);
            if (0 == ((usb_device_cdc_acm_struct_t *)handle)->hasSentState)
            {
                error = USB_DeviceCdcAcmSend(handle, USB_CIC_VCOM_IN_ENDPOINT, acmInfo->serialStateBuf, len);
                if (kStatus_USB_Success != error)
                {
                    usb_echo("kUSB_DeviceCdcEventSetControlLineState error!");
                }
                ((usb_device_cdc_acm_struct_t *)handle)->hasSentState = 1;
            }

            /* Update status */
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION)
            {
                /*  To do: CARRIER_ACTIVATED */
            }
            else
            {
                /* To do: CARRIER_DEACTIVATED */
            }
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE)
            {
                /* DTE_ACTIVATED */
                if (1 == s_UsbDeviceComposite->attach)
                {
                    s_UsbInterface0CicVcom.startTransactions = 1;
#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
    defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
    defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
                    s_waitForDataReceive = 1;
                    USB0->INTEN &= ~USB_INTEN_SOFTOKEN_MASK;
                    s_comOpen = 1;
                    usb_echo("USB_APP_CDC_DTE_ACTIVATED\r\n");
#endif
                }
            }
            else
            {
                /* DTE_DEACTIVATED */
                if (1 == s_UsbDeviceComposite->attach)
                {
                    s_UsbInterface0CicVcom.startTransactions = 0;
                }
            }
        }
        break;
        case kUSB_DeviceCdcEventSendBreak:
            break;
        default:
            break;
    }

    return error;
}

/*!
* @brief Notifies application layer about set configuration event.
*
* @param handle Handle to USB device class.
* @param configuration_idx Id of device configuration.
* @return usb_status_t Always return kStatus_USB_Success value.
*/
usb_status_t USB_DeviceInterface0CicVcomSetConfiguration(class_handle_t handle, uint8_t configuration_idx)
{
    /* Schedule buffer for receive */
    USB_DeviceCdcAcmRecv(s_UsbInterface0CicVcom.cdcAcmHandle, USB_DIC_VCOM_OUT_ENDPOINT, s_currRecvBuf,
                         g_UsbDeviceInterface1DicVcomSetting0DefaultEndpoints[USB_DIC_VCOM_OUT_ENDPOINT_INDEX].maxPacketSize);

    return kStatus_USB_Success;
}

/*!
* @brief Notifies application layer about set configuration event.
*
* @param class_handle_t Handle to USB device class.
* @param alternateSetting Id of device alternative setting.
* @return usb_status_t Always return kStatus_USB_Success value.
*/
usb_status_t USB_DeviceInterface0CicVcomSetInterface(class_handle_t handle, uint8_t alternateSetting)
{
   return kStatus_USB_Success;
}

/*!
* @brief Notifies application layer about set configuration event.
*
* @param class_handle_t Handle to USB device class.
* @param alternateSetting Id of device alternative setting.
* @return usb_status_t Always return kStatus_USB_Success value.
*/
usb_status_t USB_DeviceInterface1DicVcomSetInterface(class_handle_t handle, uint8_t alternateSetting)
{
   return kStatus_USB_Success;
}

/*!
* @brief Initializes device structure and buffer pointers.
*
* @param *device Pointer to structure to initialize to.
* @return usb_status_t Always return kStatus_USB_Success value.
*/
usb_status_t USB_DeviceInterface0CicVcomInit(usb_device_composite_struct_t *deviceComposite)
{
    s_UsbDeviceComposite = deviceComposite;
    s_UsbInterface0CicVcom.cdcAcmHandle = s_UsbDeviceComposite->interface0CicVcomHandle;
    return kStatus_USB_Success;
}
