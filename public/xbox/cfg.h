// blank file for phnt on xbox 360

typedef enum _PNP_VETO_TYPE {
  PNP_VetoTypeUnknown,
  PNP_VetoLegacyDevice,
  PNP_VetoPendingClose,
  PNP_VetoWindowsApp,
  PNP_VetoWindowsService,
  PNP_VetoOutstandingOpen,
  PNP_VetoDevice,
  PNP_VetoDriver,
  PNP_VetoIllegalDeviceRequest,
  PNP_VetoInsufficientPower,
  PNP_VetoNonDisableable,
  PNP_VetoLegacyDriver,
  PNP_VetoInsufficientRights,
  PNP_VetoAlreadyRemoved
} PNP_VETO_TYPE, *PPNP_VETO_TYPE;
