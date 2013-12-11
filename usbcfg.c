/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "usbcfg.h"

#define USB_DESCRIPTOR_RPIPE		34

/*
 * Endpoints to be used for USBD1.
 */
#define USBD1_INTERRUPT_REQUEST_EP      1

/*
 * USB Device Descriptor.
 */
static const uint8_t hid_device_descriptor_data[18] = {
  USB_DESC_DEVICE       (0x0110,        /* bcdUSB (1.1).                    */
                         0x03,          /* bDeviceClass (HID).              */
                         0x00,          /* bDeviceSubClass.                 */
                         0x00,          /* bDeviceProtocol.                 */
                         0x40,          /* bMaxPacketSize.                  */
                         0x0483,        /* idVendor (ST).                   */
                         0x5740,        /* idProduct.                       */
                         0x0200,        /* bcdDevice.                       */
                         1,             /* iManufacturer.                   */
                         2,             /* iProduct.                        */
                         3,             /* iSerialNumber.                   */
                         1)             /* bNumConfigurations.              */
};

/*
 * Device Descriptor wrapper.
 */
static const USBDescriptor hid_device_descriptor = {
  sizeof hid_device_descriptor_data,
  hid_device_descriptor_data
};

/* Configuration Descriptor tree for a HID.*/
static const uint8_t hid_configuration_descriptor_data[34] = {
  /* Configuration Descriptor.*/
  USB_DESC_CONFIGURATION(34,            /* wTotalLength.                    */
                         0x01,          /* bNumInterfaces.                  */
                         0x01,          /* bConfigurationValue.             */
                         0,             /* iConfiguration.                  */
                         0xE0,          /* bmAttributes (self powered).     */
                         50),         /* bMaxPower (100mA).               */
  /* Interface Descriptor.*/
  USB_DESC_INTERFACE    (0x00,          /* bInterfaceNumber.                */
                         0x00,          /* bAlternateSetting.               */
                         0x01,          /* bNumEndpoints.                   */
                         0x03,          /* bInterfaceClass (HID).           */
                         0x01,          /* bInterfaceSubClass : 1=BOOT,
                                           0=no boot.                       */
                         0x02,          /* nInterfaceProtocol : 0=none,
                                           1=keyboard, 2=mouse.             */
                         0),            /* iInterface.                      */
  /* Joystick Mouse HID Descriptor. */
  USB_DESC_BYTE         (9),            /* bLength.                         */
  USB_DESC_BYTE         (0x21),         /* bDescriptorType (HID).           */
  USB_DESC_WORD         (0x0100),       /* bcdHID.                          */
  USB_DESC_BYTE         (0x00),         /* bCountryCode.                    */
  USB_DESC_BYTE         (0x01),         /* bNumDescriptors.                 */
  USB_DESC_BYTE         (0x22),         /* bDescriptorType.                 */
  USB_DESC_BYTE         (74),           /* wItemLength.                     */
  USB_DESC_BYTE         (0x00),         /* iInterface.                      */
  /* Joystick Mouse Endpoint Descriptor. */
  USB_DESC_ENDPOINT     (USBD1_INTERRUPT_REQUEST_EP|0x81,
                         0x03,          /* bmAttributes (Interrupt).        */
                         0x0004,        /* wMaxPacketSize.                  */
                         0x20),         /* bInterval.                       */
};

/*
 * Configuration Descriptor wrapper.
 */
static const USBDescriptor hid_configuration_descriptor = {
  sizeof hid_configuration_descriptor_data,
  hid_configuration_descriptor_data
};

static const uint8_t hid_report_descriptor_data[74] =
  {
    0x05,          /*Usage Page(Generic Desktop)*/
    0x01,
    0x09,          /*Usage(Mouse)*/
    0x02,
    0xA1,          /*Collection(Logical)*/
    0x01,
    0x09,          /*Usage(Pointer)*/
    0x01,
    /* 8 */
    0xA1,          /*Collection(Linked)*/
    0x00,
    0x05,          /*Usage Page(Buttons)*/
    0x09,
    0x19,          /*Usage Minimum(1)*/
    0x01,
    0x29,          /*Usage Maximum(3)*/
    0x03,
    /* 16 */
    0x15,          /*Logical Minimum(0)*/
    0x00,
    0x25,          /*Logical Maximum(1)*/
    0x01,
    0x95,          /*Report Count(3)*/
    0x03,
    0x75,          /*Report Size(1)*/
    0x01,
    /* 24 */
    0x81,          /*Input(Variable)*/
    0x02,
    0x95,          /*Report Count(1)*/
    0x01,
    0x75,          /*Report Size(5)*/
    0x05,
    0x81,          /*Input(Constant,Array)*/
    0x01,
    /* 32 */
    0x05,          /*Usage Page(Generic Desktop)*/
    0x01,
    0x09,          /*Usage(X axis)*/
    0x30,
    0x09,          /*Usage(Y axis)*/
    0x31,
    0x09,          /*Usage(Wheel)*/
    0x38,
    /* 40 */
    0x15,          /*Logical Minimum(-127)*/
    0x81,
    0x25,          /*Logical Maximum(127)*/
    0x7F,
    0x75,          /*Report Size(8)*/
    0x08,
    0x95,          /*Report Count(3)*/
    0x03,
    /* 48 */
    0x81,          /*Input(Variable, Relative)*/
    0x06,
    0xC0,          /*End Collection*/
    0x09,
    0x3c,
    0x05,
    0xff,
    0x09,
    /* 56 */
    0x01,
    0x15,
    0x00,
    0x25,
    0x01,
    0x75,
    0x01,
    0x95,
    /* 64 */
    0x02,
    0xb1,
    0x22,
    0x75,
    0x06,
    0x95,
    0x01,
    0xb1,
    /* 72 */
    0x01,
    0xc0
  }
  ;

/*
 * Report Descriptor wrapper.
 */
static const USBDescriptor hid_report_descriptor = {
  sizeof hid_report_descriptor_data,
  hid_report_descriptor_data
};

/*
 * U.S. English language identifier.
 */
static const uint8_t hid_string0[] = {
  USB_DESC_BYTE(4),                     /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  USB_DESC_WORD(0x0409)                 /* wLANGID (U.S. English).          */
};

/*
 * Vendor string.
 */
static const uint8_t hid_string1[] = {
  USB_DESC_BYTE(16),                    /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  'L', 0, 'u', 0, 's', 0, 'o', 0, 'r', 0, 'i', 0, 'a', 0
};

/*
 * Device Description string.
 */
static const uint8_t hid_string2[] = {
  USB_DESC_BYTE(8),                    /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  'J', 0, 'o', 0, 'y', 0
};

/*
 * Serial Number string.
 */
static const uint8_t hid_string3[] = {
  USB_DESC_BYTE(8),                     /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  '0', 0,
  '0', 0,
  '1', 0
};

/*
 * Strings wrappers array.
 */
static const USBDescriptor hid_strings[] = {
  {sizeof hid_string0, hid_string0},
  {sizeof hid_string1, hid_string1},
  {sizeof hid_string2, hid_string2},
  {sizeof hid_string3, hid_string3}
};

/*
 * Handles the GET_DESCRIPTOR callback. All required descriptors must be
 * handled here.
 */
static const USBDescriptor *get_descriptor(USBDriver *usbp,
                                           uint8_t dtype,
                                           uint8_t dindex,
                                           uint16_t lang) {

  (void)usbp;
  (void)lang;
  switch (dtype) {
  case USB_DESCRIPTOR_DEVICE:
    return &hid_device_descriptor;
  case USB_DESCRIPTOR_CONFIGURATION:
    return &hid_configuration_descriptor;
  case USB_DESCRIPTOR_STRING:
    if (dindex < 4)
      return &hid_strings[dindex];
    else
      break;
  case 0x21:
	  while(1);
	break;
  case 0x22:
	  while(1);
	  return &hid_report_descriptor;
  default:
	  while(1);
  }
  return NULL;
}

/**
 * @brief   IN EP1 state.
 */
static USBInEndpointState ep1instate;

/**
 * @brief   OUT EP1 state.
 */
static USBOutEndpointState ep1outstate;

/**
 * @brief   EP1 initialization structure (both IN and OUT).
 */
static const USBEndpointConfig ep1config = {
  USB_EP_MODE_TYPE_BULK,
  NULL,
  sduDataTransmitted,
  sduDataReceived,
  0x0040,
  0x0040,
  &ep1instate,
  &ep1outstate,
  2,
  NULL
};

/**
 * @brief   IN EP2 state.
 */
static USBInEndpointState ep2instate;

/**
 * @brief   EP2 initialization structure (IN only).
 */
static const USBEndpointConfig ep2config = {
  USB_EP_MODE_TYPE_INTR,
  NULL,
  sduInterruptTransmitted,
  NULL,
  0x0010,
  0x0000,
  &ep2instate,
  NULL,
  1,
  NULL
};

/*
 * Handles the USB driver global events.
 */
static void usb_event(USBDriver *usbp, usbevent_t event) {
  extern SerialUSBDriver SDU1;

  switch (event) {
  case USB_EVENT_RESET:
    return;
  case USB_EVENT_ADDRESS:
    return;
  case USB_EVENT_CONFIGURED:
    chSysLockFromIsr();

    /* Enables the endpoints specified into the configuration.
       Note, this callback is invoked from an ISR so I-Class functions
       must be used.*/
//    usbInitEndpointI(usbp, USBD1_DATA_REQUEST_EP, &ep1config);
//    usbInitEndpointI(usbp, USBD1_INTERRUPT_REQUEST_EP, &ep2config);

    /* Resetting the state of the CDC subsystem.*/
//    sduConfigureHookI(&SDU1);

    chSysUnlockFromIsr();
    return;
  case USB_EVENT_SUSPEND:
    return;
  case USB_EVENT_WAKEUP:
    return;
  case USB_EVENT_STALLED:
    return;
  }
  return;
}

/*
 * USB driver configuration.
 */
const USBConfig usbcfg = {
  usb_event,
  get_descriptor,
  sduRequestsHook,
  NULL
};


/*
 * Serial over USB driver configuration.
 */
const HIDUSBConfig hidcfg = {
  &USBD1,
  USBD1_INTERRUPT_REQUEST_EP,
};
