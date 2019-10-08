#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE* SystemTable)
{
	EFI_STATUS Status;
	EFI_GUID GuidFS = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	UINTN handleCount = 0;
	EFI_HANDLE* handles = NULL;
	EFI_FILE_PROTOCOL* root = NULL, * token = NULL;;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs = NULL;
	UINTN BufferSize = 0x2000, i;
	
	//
	// memory allocation
	//
	void * Page = AllocatePool(0x2000);
	if (Page == NULL)
	{
		Print(L"ERROR: Impossible d'allouer la mémoire\n");
		return EFI_SUCCESS;
	}

	//
	// set memory 
	//
	for (i = 0; i < 0x2000; i++)
	{
		((CHAR8*)Page)[i] = 0xAA;
	}
	
	//
	// Find Handle for file system
	//
	Status = gBS->LocateHandleBuffer(ByProtocol, &GuidFS, NULL, &handleCount, &handles);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR: Impossible de reuperer le handle du file system\n");
		return Status;
	}

	//
	// Retrieve the protoole associate to the Handle
	//
	Status = gBS->HandleProtocol(handles[0], &GuidFS, (void**)&fs);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR: Impossible de reuperer le protocole du file system\n");
		return Status;
	}

	//
	// Open first Fat32 volume
	//
	Status = fs->OpenVolume(fs, &root);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR: Impossible d'ouvrir ma racine\n");
		return Status;
	}

	//
	// Create/Retrieve file 
	//
	Status = root->Open(root, &token, L"\\Alpha.txt ", EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR: Impossible d'ouvrir mon fichier\n");
		return Status;
	}

	//
	// Writing funtion used for the IO debug
	//
	for (i=0;i<10;i++)
	{
		__debugbreak();
		Status = token->Write(token, &BufferSize, Page);
		if (EFI_ERROR(Status))
		{
			Print(L"ERROR: Impossible d'écrire dans mon fichier\n");
			return Status;
		}
	}

	//
	// Close file
	//
	Status = token->Close(token);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR: Impossible de fermer dans mon fichier\n");
		return Status;
	}

	return EFI_SUCCESS;
}
