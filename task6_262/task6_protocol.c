/* TASK6_PROTOCOL.C -----------------------------------------------------------

   PG3401 EXAM 2026 Spring
   Author: candidate 262

   Description: Handles most of the Task 6 protocol work. It receives
   packets from EWA, reads the custom header and data, answers with ACK/NACK,
   stores packets by sequence number, and finally writes the received butterfly
   to task6_received.bmp. File also contains the checksum and packet-list
   helper functions used by the receive loop.
    
   This file contains the main problem-solving part of the task. During
   development it was used together with EWA feedback and debug printouts to
   test packet sizes, sequence handling, ACK/NACK responses, checksum handling,
   out-of-sequence packets, and writing the received file in the right order.


------------------------------------------------------------------------------*/

/**** Includes ****************************************************************/

   /* Standard C libraries*/
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>

   /* Linux/socket related libraries*/
   #include <sys/socket.h>
   #include <unistd.h>

   /* Task specific libraries*/
   #include "ewpdef.h"
   #include "task6_protocol.h"

/*******************************************************************************
**** PACKET STORAGE STRUCT *****************************************************
*******************************************************************************/

/* Linked list node used to store received packets until they can be written
   to task6_received.bmp in right sequence order*/
struct PACKET_NODE {
   unsigned int uiSequenceNumber;
   int iDataSize;
   unsigned char *pData;
   struct PACKET_NODE *pNext;
};

/**** CHECKSUM METHODS  *******************************************************/
unsigned short CalculateChecksum(unsigned char *pData, int iLength) {
   unsigned long ulSum = 0;
   unsigned short usWord = 0;
   int i = 0;

   /* Add all 16-bit words as big endian values*/
   for (i = 0; i + 1 < iLength; i += 2) {
      usWord = (unsigned short)((pData[i] << 8) + pData[i + 1]);
      ulSum += usWord;
   }

   /* Add trailing odd byte if present*/
   if (i < iLength) {
      usWord = (unsigned short)(pData[i] << 8);
      ulSum += usWord;
   }

   /* Fold carry bits back into 16-bit checksum*/
   while (ulSum >> 16) {
      ulSum = (ulSum & 0xFFFF) + (ulSum >> 16);
   }

   /* Return one's complement*/
   return (unsigned short)(~ulSum);
}

/**** Calculate checksum using little endian 16-bit ***************************/
unsigned short CalculateChecksumLittle(unsigned char *pData, int iLength) {
   unsigned long ulSum = 0;
   unsigned short usWord = 0;
   int i = 0;

   /* Add all 16-bit words as little endian values*/
   for (i = 0; i + 1 < iLength; i += 2) {
      usWord = (unsigned short)(pData[i] + (pData[i + 1] << 8));
      ulSum += usWord;
   }

   /* Add trailing odd byte if present*/
   if (i < iLength) {
      usWord = (unsigned short)pData[i];
      ulSum += usWord;
   }

   /* Fold carry bits back into 16-bit checksum*/
   while (ulSum >> 16) {
      ulSum = (ulSum & 0xFFFF) + (ulSum >> 16);
   }

   /* Return one's complement*/
   return (unsigned short)(~ulSum);
}

/*******************************************************************************
**** PACKET STORAGE METHODS ****************************************************
*******************************************************************************/

/**** Add packet data into linked list sorted by sequence number **************/
int AddPacketSorted(struct PACKET_NODE **ppHead,
                    unsigned int uiSequenceNumber,
                    unsigned char *pData,
                    int iDataSize) {
   struct PACKET_NODE *pNewNode = NULL;
   struct PACKET_NODE *pCurrent = NULL;

   /* Validate input pointers*/
   if (ppHead == NULL || pData == NULL) {
      return -1;
   }

   /* Allocate new linked list node*/
   pNewNode = (struct PACKET_NODE *)malloc(sizeof(struct PACKET_NODE));
   if (pNewNode == NULL) {
      puts("ERROR: Failed to allocate packet node.");
      return -1;
   }

   /* Fill new node with packet information*/
   pNewNode->uiSequenceNumber = uiSequenceNumber;
   pNewNode->iDataSize = iDataSize;
   pNewNode->pData = pData;
   pNewNode->pNext = NULL;

   /* Insert first node if list is empty*/
   if (*ppHead == NULL) {
      *ppHead = pNewNode;
      return 0;
   }

    /* Insert before current head if new sequence is smaller*/
   if (uiSequenceNumber < (*ppHead)->uiSequenceNumber) {
      pNewNode->pNext = *ppHead;
      *ppHead = pNewNode;
      return 0;
   }

   /* Ignore duplicate packet if same sequence already exists*/
   if (uiSequenceNumber == (*ppHead)->uiSequenceNumber) {
      free(pNewNode);
      return 1;
   }

    /* Find correct sorted position in list*/
   pCurrent = *ppHead;
   while (pCurrent->pNext != NULL &&
          pCurrent->pNext->uiSequenceNumber < uiSequenceNumber) {
      pCurrent = pCurrent->pNext;
   }

   /* Ignore duplicate packet if same sequence already exists*/
   if (pCurrent->pNext != NULL &&
       pCurrent->pNext->uiSequenceNumber == uiSequenceNumber) {
      free(pNewNode);
      return 1;
   }

   /* Insert new node into sorted position*/
   pNewNode->pNext = pCurrent->pNext;
   pCurrent->pNext = pNewNode;

   return 0;
}

/**** Write stored packets to BMP file in sequence order ************************/
int WritePacketsToFile(struct PACKET_NODE *pHead) {
   FILE *fOutput = NULL;
   struct PACKET_NODE *pCurrent = NULL;
   unsigned int uiExpectedSequence = 0;

   /* Create output BMP file in binary mode*/
   fOutput = fopen("task6_received.bmp", "wb");
   if (fOutput == NULL) {
      puts("ERROR: Failed to create task6_received.bmp.");
      return -1;
   }

   /* Write each stored packet in sorted sequence order*/
   pCurrent = pHead;
   while (pCurrent != NULL) {

      /* Warn if a sequence gap is found*/
      if (pCurrent->uiSequenceNumber != uiExpectedSequence) {
         printf("WARNING: sequence gap before %u, expected %u\n",
                pCurrent->uiSequenceNumber,
                uiExpectedSequence);
      }

      /* Write packet data to BMP file */
      if (fwrite(pCurrent->pData,
                 1,
                 pCurrent->iDataSize,
                 fOutput) != (size_t)pCurrent->iDataSize) {
         puts("ERROR: Failed to write packet data to BMP file.");
         fclose(fOutput);
         return -1;
      }

      /* Move expected sequence forward */
      uiExpectedSequence = pCurrent->uiSequenceNumber + pCurrent->iDataSize;

      /* Move to next stored packet */
      pCurrent = pCurrent->pNext;
   }

   /* Close BMP output file */
   fclose(fOutput);
   puts("Saved file as task6_received.bmp.");

   return 0;
}

/*** Freeing all stored packet nodes and packet data***************************/
void FreePackets(struct PACKET_NODE *pHead) {
   struct PACKET_NODE *pCurrent = NULL;
   struct PACKET_NODE *pNext = NULL;

   /* Walk through list and free all packet data and nodes*/
   pCurrent = pHead;
   while (pCurrent != NULL) {
      pNext = pCurrent->pNext;

      /* Free packet data buffer under this node*/
      if (pCurrent->pData != NULL) {
         free(pCurrent->pData);
         pCurrent->pData = NULL;
      }

      /* Free the node itself*/
      free(pCurrent);

      /* Continue to next node*/
      pCurrent = pNext;
   }
}

/*******************************************************************************
*** PROTOCOL METHODS ***********************************************************
*******************************************************************************/
unsigned short Swap16(unsigned short usValue) {
   unsigned short usResult = 0;

   /* Swap low and high byte */ 
   usResult = (unsigned short)(((usValue & 0x00FF) << 8) |
			       ((usValue & 0xFF00) >> 8));
   return usResult;
}

/*** Send simple 20-byte ACK packet back to EWA *******************************/
int SendAckTest(int iSocket, 
	struct EWA_EXAM25_TASK4_PROTOCOL_TCP *pReceivedHeader) {
   int SendStatus = 0;
   unsigned int uiAckNumber = 0;
   unsigned short usChecksum = 0;
   struct EWA_EXAM25_TASK4_PROTOCOL_TCP stAck;

   /* Clear ACK struct before filling in fields */
   memset(&stAck, 0, sizeof(stAck));

   /* Set ACK number to tell EWA what we accepted */
   stAck.usSourcePort = pReceivedHeader->usDestinationPort;
   stAck.usDestinationPort = pReceivedHeader->usSourcePort; 


   stAck.uiSequenceNumber = 0;

   uiAckNumber = pReceivedHeader->uiSequenceNumber;
   stAck.uiAckNumber = uiAckNumber;

   stAck.ucDataOffset = 5;
   stAck.ucReserved = 0;

   stAck.ucFlags = FLAG_ACK;
   
   stAck.usSizeOfPacket = 0;

   stAck.usChecksum = 0;

   stAck.usUnused = 0;   
  
   usChecksum = CalculateChecksum((unsigned char *)&stAck, PROTOCOL_HEADER_SIZE);

   stAck.usChecksum = Swap16(usChecksum);

   /* Printed sent ACK header fields for debugging*/
   printf("ACK Source port:	%u\n", stAck.usSourcePort);
   printf("ACK Destination port:%u\n", stAck.usDestinationPort);
   printf("ACK Sequence number:	%u\n", stAck.uiSequenceNumber);
   printf("ACK Data offset:	%u\n", stAck.ucDataOffset);
   printf("ACK Reserved:	%u\n", stAck.ucReserved);	
   printf("ACK Flags:		0x%02X\n", stAck.ucFlags);
   printf("ACK Size:		%u\n", stAck.usSizeOfPacket);
   printf("ACK Checksum: 	0x%04X\n", stAck.usChecksum);


   /* Send 20 byte header part of protocol struct, defined in protocol header*/
   SendStatus = send(iSocket, &stAck, PROTOCOL_HEADER_SIZE, 0);

   /* Check that EWA received a complete 20-byte ACK header */
   if (SendStatus != PROTOCOL_HEADER_SIZE) {
      puts("ERROR: Failed to send ACK test packet.");
      return -1;
   }

   printf("Sent ACK packet. AckNumber: %u Checksum: 0x%04X\n", stAck.uiAckNumber, stAck.usChecksum);

   return 0;
}

/*** Send 20 byte NACK packet back to EWA *************************************/
int SendNack(int iSocket,
             struct EWA_EXAM25_TASK4_PROTOCOL_TCP *pReceivedHeader) {
   int SendStatus = 0;
   unsigned short usChecksum = 0;
   struct EWA_EXAM25_TASK4_PROTOCOL_TCP stNack;

   /* Clear NACK struct before filling in fields */
   memset(&stNack, 0, sizeof(stNack));

   /* NACK answers from our port back to EWA port */
   stNack.usSourcePort = pReceivedHeader->usDestinationPort;
   stNack.usDestinationPort = pReceivedHeader->usSourcePort;

   /* Sequence number is zero for this simple response packet */
   stNack.uiSequenceNumber = 0;

   /* ACK number identifies the packet sequence that should be resent */
   stNack.uiAckNumber = pReceivedHeader->uiSequenceNumber;

   /* Set TCP-like 20-byte header */
   stNack.ucDataOffset = 5;
   stNack.ucReserved = 0;

   /* Use URG flag value as NACK, as specified in ewpdef.h comments */
   stNack.ucFlags = FLAG_NACK;

   /* NACK packet has no payload */
   stNack.usSizeOfPacket = 0;

   /* Checksum field must be zero before checksum calculation*/
   stNack.usChecksum = 0;

   /* Unused field remains zero*/
   stNack.usUnused = 0;

   /* Calculate checksum over 20-byte response header*/
   usChecksum = CalculateChecksum((unsigned char *)&stNack,
                                  PROTOCOL_HEADER_SIZE);

   /* Store checksum using the same method that EWA accepted for ACK*/
   stNack.usChecksum = usChecksum;

   /* Send only 20-byte response header*/
   SendStatus = send(iSocket, &stNack, PROTOCOL_HEADER_SIZE, 0);

   /* Validate that full NACK header was sent*/
   if (SendStatus != PROTOCOL_HEADER_SIZE) {
      puts("ERROR: Failed to send NACK packet.");
      return -1;
   }

   printf("Sent NACK for sequence: %u\n", stNack.uiAckNumber);

   return 0;
}

/*** Receive full BMP file from EWA *******************************************/
int ReceiveFileFromEwa(int iSocket) {
   int ReceiveStatus = 0;
   int AckStatus = 0;
   int AddStatus = 0;
   int WriteStatus = 0;
   int iDataSize = 0;
   int Done = 0;
   unsigned char *pData = NULL;
   struct PACKET_NODE *pPacketList = NULL;
   struct EWA_EXAM25_TASK4_PROTOCOL_TCP stHeader;

   /* Continue until FIN packet is received*/
   while (Done == 0) {

      /* Clear header before receiving new packet*/
      memset(&stHeader, 0, sizeof(stHeader));

      /* Receive 20-byte protocol header */
      ReceiveStatus = recv(iSocket, &stHeader, PROTOCOL_HEADER_SIZE, 0);

      /* Validate that full header was received*/
      if (ReceiveStatus != PROTOCOL_HEADER_SIZE) {
         puts("ERROR: Failed to receive packet header.");
         FreePackets(pPacketList);
         return -1;
      }

      /* Read packet data size from header*/
      iDataSize = stHeader.usSizeOfPacket;

      /* Validate packet data size before allocation*/
      if (iDataSize < 0 || iDataSize > MAX_PACKET_DATA_SIZE) {
         puts("ERROR: Invalid packet data size.");
         FreePackets(pPacketList);
         return -1;
      }

      /* Allocate packet data buffer if packet contains data*/
      if (iDataSize > 0) {
         pData = (unsigned char *)malloc(iDataSize);
         if (pData == NULL) {
            puts("ERROR: Failed to allocate packet data.");
            FreePackets(pPacketList);
            return -1;
         }

         /* Clear packet data buffer before receiving*/
         memset(pData, 0, iDataSize);

         /* Receive packet payload*/
         ReceiveStatus = recv(iSocket, pData, iDataSize, 0);

         /* Check that full packet payload was received*/
         if (ReceiveStatus != iDataSize) {
            puts("ERROR: Failed to receive complete packet.");
            free(pData);
            pData = NULL;
            FreePackets(pPacketList);
            return -1;
         }
      } else {
         pData = NULL;
      }

      /* Print received packet information */
      printf("Packet sequence: %u size: %d flags: 0x%02X\n",
             stHeader.uiSequenceNumber,
             iDataSize,
             stHeader.ucFlags);

      /* Store packet data for later writing in sequence order*/
      if (iDataSize > 0) {
         AddStatus = AddPacketSorted(&pPacketList,
                                     stHeader.uiSequenceNumber,
                                     pData,
                                     iDataSize);

         /* Validate that packet was stored or recognized as duplicate*/
         if (AddStatus < 0) {
            puts("ERROR: Failed to store packet.");
            free(pData);
            pData = NULL;
            FreePackets(pPacketList);
            return -1;
         }

         /* Duplicate packet: list did not take ownership, so free it here*/
         if (AddStatus == 1) {
            puts("INFO: Duplicate packet ignored.");
            free(pData);
            pData = NULL;
         } else {
            /* Packet list now owns pData */
            pData = NULL;
         }
      }

      /* Sends ACK for all received valid packets and those out of sequence*/
      AckStatus = SendAckTest(iSocket, &stHeader);
      if (AckStatus < 0) {
         if (pData != NULL) {
            free(pData);
            pData = NULL;
         }
         FreePackets(pPacketList);
         return -1;
      }

      /* Check if this packet has FIN flag set*/
      if ((stHeader.ucFlags & FLAG_FIN) != 0) {
         puts("FIN received, file transfer complete.");
         Done = 1;
      }
   }

   /* Writes all stored packets to BMP file in sequence order*/
   WriteStatus = WritePacketsToFile(pPacketList);
   if (WriteStatus < 0) {
      FreePackets(pPacketList);
      return -1;
   }

   /* Free all packets after writing file*/
   FreePackets(pPacketList);
   pPacketList = NULL;

   return 0;
}

/*** Receive first packet, print values and send ACK **************************/

int ReceiveFirstPacketTest(int iSocket) {
   int ReceiveStatus = 0;
   int SendStatus = 0;
   int iDataSize = 0;
   unsigned char *pData = NULL;
   struct EWA_EXAM25_TASK4_PROTOCOL_TCP stHeader;
   unsigned char *pFullPacket = NULL;
   struct EWA_EXAM25_TASK4_PROTOCOL_TCP stChecksumHeader;
   unsigned short usCalcBig = 0;
   unsigned short usCalcLittle = 0;
   unsigned short usReceivedRaw = 0;
   int iPacketSize = 0;

   /* Clear header struct before receiving into it*/
   memset(&stHeader, 0, sizeof(stHeader));

   /* Receive only the 20-byte fixed header first*/
   ReceiveStatus = recv(iSocket, &stHeader, PROTOCOL_HEADER_SIZE, 0);

   /* Validate that a full header was received*/
   if (ReceiveStatus != PROTOCOL_HEADER_SIZE) {
      puts("ERROR: Failed to receive first packet header.");
      return -1;
   }

   /* Print values from received header for debugging*/
   printf("ACK Source port:      %u\n", stHeader.usSourcePort);
   printf("ACK Destination port: %u\n", stHeader.usDestinationPort);
   printf("ACK Sequence number:  %u\n", stHeader.uiSequenceNumber);
   printf("Ack number:       	 %u\n", stHeader.uiAckNumber);
   printf("ACK Data offset:      %u\n", stHeader.ucDataOffset);
   printf("ACK Reserved:	 %u\n", stHeader.ucReserved);
   printf("ACK Flags:            0x%02X\n", stHeader.ucFlags);
   printf("ACK Data size:        %u\n", stHeader.usSizeOfPacket);
   printf("ACK Checksum:         0x%04X\n", stHeader.usChecksum);

   /* Save packet data size from header*/
   iDataSize = stHeader.usSizeOfPacket;

   /* Validate packet data size before allocation*/
   if (iDataSize < 0 || iDataSize > MAX_PACKET_DATA_SIZE) {
      puts("ERROR: Invalid packet data size.");
      return -1;
   }

   /* Allocate memory for packet data*/
   pData = (unsigned char *)malloc(iDataSize);
   if (pData == NULL) {
      puts("ERROR: Failed to allocate packet data.");
      return -1;
   }

   /* Receive packet data bytes*/
   ReceiveStatus = recv(iSocket, pData, iDataSize, 0);

   /* Validate that expected packet data bytes were received*/
   if (ReceiveStatus != iDataSize) {
      puts("ERROR: Failed to receive complete packet data.");
      free(pData);
      pData = NULL;
      return -1;
   }

   /* Print first few bytes to confirm binary file data is arriving*/
   if (iDataSize >= 2) {
      printf("First data bytes: 0x%02X 0x%02X\n", pData[0], pData[1]);
   }
   
   /* Save received checksum before clearing checksum field*/
   usReceivedRaw = stHeader.usChecksum;

   /* Copy header so checksum can be calculated with checksum field as zero*/
   memcpy(&stChecksumHeader, &stHeader, PROTOCOL_HEADER_SIZE);
   stChecksumHeader.usChecksum = 0;

   /* Calculate full received packet size*/
   iPacketSize = PROTOCOL_HEADER_SIZE + iDataSize;

   /* Allocate temporary buffer for header plus data */
   pFullPacket = (unsigned char *)malloc(iPacketSize);
   if (pFullPacket == NULL) {
      puts("ERROR: Failed to allocate checksum test buffer.");
      free(pData);
      pData = NULL;
      return -1;
   }

   /* Copy checksum header into full packet buffer*/
   memcpy(pFullPacket, &stChecksumHeader, PROTOCOL_HEADER_SIZE);

   /* Copy packet data after header*/
   memcpy(pFullPacket + PROTOCOL_HEADER_SIZE, pData, iDataSize);

   /* Calculate checksum using big endian*/
   usCalcBig = CalculateChecksum(pFullPacket, iPacketSize);

   /* Calculate checksum using little endian*/
   usCalcLittle = CalculateChecksumLittle(pFullPacket, iPacketSize);

   /* Debugging, print checksum values to compare against EWA packet*/
   printf("RX checksum raw:       0x%04X\n", usReceivedRaw);
   printf("RX checksum swapped:   0x%04X\n", Swap16(usReceivedRaw));
   printf("Calc checksum big:     0x%04X\n", usCalcBig);
   printf("Calc checksum big swp: 0x%04X\n", Swap16(usCalcBig));
   printf("Calc checksum little:  0x%04X\n", usCalcLittle);
   printf("Calc checksum lit swp: 0x%04X\n", Swap16(usCalcLittle));

   /* Free temporary checksum buffer*/
   free(pFullPacket);
   pFullPacket = NULL;


   /* Send simple ACK for first packet*/
   SendStatus = SendAckTest(iSocket, &stHeader);
   if (SendStatus < 0) {
      free(pData);
      pData = NULL;
      return -1;
   }
   
   /* Clears & accepts next 20 byte EWA protocol header*/
   memset(&stHeader, 0, sizeof(stHeader));

   ReceiveStatus = recv(iSocket, &stHeader, PROTOCOL_HEADER_SIZE, 0);

   /* Debug check for next packet after ACK*/
   if (ReceiveStatus == PROTOCOL_HEADER_SIZE) {
      puts("Received next packet header after ACK.");
      printf("Next sequence number: %u\n", stHeader.uiSequenceNumber);
      printf("Next data size:       %u\n", stHeader.usSizeOfPacket);
      printf("Nextflags:            0x%02X\n", stHeader.ucFlags);
   } else {
      printf("Did not receive next header. recv returned %d\n", ReceiveStatus);
   }

   /* Free packet data buffer before returning*/
   free(pData);
   pData = NULL;

   return 0;
}  

/* EOF -----------------------------------------------------------------------*/
