#include <efi.h>
#include <efilib.h>
#include <efiprot.h>
#include "draw.h"

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

EFI_STATUS get_mode(EFI_GRAPHICS_OUTPUT_PROTOCOL *graphics, UINT32 offset, GraphicsSettings *settings) {
  EFI_STATUS result;
  UINT32 mode;
  UINTN size;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *modeInfo;

  mode = graphics->Mode->Mode;

  for (UINT32 i = 0; i < offset; i++)
  {
    result = uefi_call_wrapper(graphics->QueryMode, 4, graphics, mode, &size, &modeInfo);

    settings->width = modeInfo->HorizontalResolution;
    settings->height = modeInfo->VerticalResolution;
    settings->mode = mode;

    Print(L"%02d (%02d): %dx%d pixel format: %d\n", i, mode, settings->width, settings->height, modeInfo->PixelFormat);
  }

  return result;
}

EFI_STATUS print_modes(EFI_GRAPHICS_OUTPUT_PROTOCOL *graphics)
{
  GraphicsSettings settings;

  for (UINT32 i = 0; i < graphics->Mode->MaxMode; i++)
  {
    get_mode(graphics, i, &settings);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  InitializeLib(ImageHandle, SystemTable);
  uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"Loading graphics...\n");

  EFI_GRAPHICS_OUTPUT_PROTOCOL *graphics;
  EFI_GUID graphics_proto = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

  uefi_call_wrapper(SystemTable->BootServices->LocateProtocol, 3, &graphics_proto, NULL, &graphics);
//  print_modes(graphics);

  GraphicsSettings settings;
  get_mode(graphics, 1, &settings);
  uefi_call_wrapper(graphics->SetMode, 2, graphics, settings.mode);

  void *fb_base_addr = (void *)graphics->Mode->FrameBufferBase;
  void *back_buffer_addr = AllocateZeroPool(graphics->Mode->FrameBufferSize);

  draw_full(back_buffer_addr, 100, 100, 100, &settings);

  EFI_INPUT_KEY key;
  EFI_STATUS status;
  unsigned int x = 100, y = 100, py = 100, x0 = x, y0 = y, py0 = py;
  char dx = 1, dy = 1;

  do {
    CopyMem(fb_base_addr, back_buffer_addr, graphics->Mode->FrameBufferSize);

    x0 = x;
    y0 = y;
    py0 = py;

    x += dx;
    y += dy;

    if(x < 20) {
      x = 20;
      dx = 1;
    } else if(x > settings.width - 30) {
      x = settings.width - 30;
      dx = -1;
    }

    if (y < 20)
    {
      y = 20;
      dy = 1;
    }
    else if (y > settings.height - 30)
    {
      y = settings.height - 30;
      dy = -1;
    }

    draw_delta(back_buffer_addr, x0, y0, x, y, py0, py, &settings);

    status = uefi_call_wrapper(SystemTable->ConIn->ReadKeyStroke, 2, SystemTable->ConIn, &key);
    if(status == EFI_SUCCESS) {
      if(key.ScanCode == SCAN_UP) {
        py = max(py - 10, 20);
      } else {
        py = min(py + 10, settings.height - 80);
      }
    }
  } while(status != EFI_SUCCESS || key.ScanCode != SCAN_NULL);

  uefi_call_wrapper(SystemTable->RuntimeServices->ResetSystem, 4, EfiResetCold, EFI_SUCCESS, 0, NULL);

  return EFI_SUCCESS;
}
