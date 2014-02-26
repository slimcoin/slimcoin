// Copyright (c) 2012-2013 The SLIMCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef SMCOIN_KERNEL_H
#define SMCOIN_KERNEL_H

#include "main.h"
#include "wallet.h"

// MODIFIER_INTERVAL: time to elapse before new modifier is computed
static const unsigned int MODIFIER_INTERVAL = 6 * 60 * 60;
extern unsigned int nModifierInterval;

// MODIFIER_INTERVAL_RATIO:
// ratio of group interval length between the last group and the first group
static const int MODIFIER_INTERVAL_RATIO = 3;

// Protocol switch time of v0.3 kernel protocol
extern unsigned int nProtocolV03SwitchTime;
extern unsigned int nProtocolV03TestSwitchTime;

// Whether a given coinstake is subject to new v0.3 protocol
bool IsProtocolV03(unsigned int nTimeCoinStake);

// Compute the hash modifier for proof-of-stake
bool ComputeNextStakeModifier(const CBlockIndex* pindexPrev, uint64& nStakeModifier, bool& fGeneratedStakeModifier);

// Check whether stake kernel meets hash target
// Sets hashProofOfStake on success return
bool CheckStakeKernelHash(unsigned int nBits, const CBlock& blockFrom, unsigned int nTxPrevOffset, 
                          const CTransaction& txPrev, const COutPoint& prevout, unsigned int nTimeTx,
                          uint256& hashProofOfStake, bool fPrintProofOfStake=false);

// Check kernel hash target and coinstake signature
// Sets hashProofOfStake on success return
bool CheckProofOfStake(const CTransaction& tx, unsigned int nBits, uint256& hashProofOfStake);

// Check whether the coinstake timestamp meets protocol
bool CheckCoinStakeTimestamp(int64 nTimeBlock, int64 nTimeTx);

// Get stake modifier checksum
unsigned int GetStakeModifierChecksum(const CBlockIndex* pindex);

// Check stake modifier hard checkpoints
bool CheckStakeModifierCheckpoints(int nHeight, unsigned int nStakeModifierChecksum);


//////////////////////////////////////////////////////////////////////////////
/*                              Proof Of Burn                               */
//////////////////////////////////////////////////////////////////////////////

//~ #define BURN_CONSTANT 10.0 * COIN

//these numbers need to be doubles
#define BURN_CONSTANT     .01 * CENT
#define BURN_HASH_DOUBLE  1000.0  //the hash of a burnt tx doubles smoothly over the course of 1000 blocks
#define BURN_HASH_COUNT   1000    //the amount of hashes to be done when getting the smallest hash
#define BURN_MIN_CONFIRMS 1       //a burn transaction requires atleast 1 confimation

extern const CBitcoinAddress burnOfficialAddress;
extern const CBitcoinAddress burnTestnetAddress;

//Scans all of the hashes of this transaction and returns the smallest one
bool ScanBurnHashes(const CWalletTx &burnWTx, uint256 &smallestHashRet);

//Applies ScanBurnHashes to all of the burnt hashes stored in the setBurnHashes
std::pair<uint256, CWalletTx> HashAllBurntTx();

#endif // SMCOIN_KERNEL_H
