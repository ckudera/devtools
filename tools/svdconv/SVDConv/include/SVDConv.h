/*
 * Copyright (c) 2010-2022 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef SvdConv_H
#define SvdConv_H

#include "SvdOptions.h"
#include "SvdDevice.h"
#include "ErrLog.h"
#include <nlohmann/json.hpp>

#include <string>
#include <set>
#include <list>

using json = nlohmann::json;


typedef enum SvdErr_t
{
  SVD_ERR_SUCCESS = 0,       // Success
  SVD_ERR_INVALID_PARAM = 1, // Invalid param, e.g. an unallowed NULL pointer
  SVD_ERR_NO_INPATH = 2,     // No input path specified
  SVD_ERR_VERIFY = 4,        // One or more verification actions failed
  SVD_ERR_NOT_FOUND = 8,     // File not found
  SVD_ERR_INTERNAL_ERR = 11  // Internal Error
} SVD_ERR;


class SvdConv {
public:
  SvdConv();
  ~SvdConv();

  int Check(int argc, const char* argv[], const char* envp[]);
  SVD_ERR CheckSvdFile();

  void PrintDevice(SvdDevice *device);
  void PrintPeripheral(SvdPeripheral *const peri, const std::list<SvdInterrupt *> &interrupts);
  void PrintRegistersContainer(SvdPeripheral *const peri);
  void PrintRegistersClusters(const std::list<SvdItem *> &childs, int indent);
  void PrintRegisterWrapper(SvdRegister *reg, int indent);
  void PrintFieldWrapper(SvdField *const field, int indent);
  void PrintEnumContainerWrapper(SvdEnumContainer *const enumContainer, int indent);
  void PrintEnum(SvdEnum *const &enum_, int indent);
  void PrintEnumContainer(SvdEnumContainer *const enumContainer, int indent);
  void PrintClusterWrapper(SvdCluster *cluster, int indent);
  void PrintCluster(SvdCluster *cluster, int indent);
  void PrintRegister(SvdRegister *reg, int indent);
  void PrintInterrupts(const std::list<SvdInterrupt *> &interrupts);
  void PrintAddressBlocks(SvdPeripheral *const peri);
  void PrintField(SvdField *field, int indent);

  json PeripheralToJson(SvdPeripheral *const peri, const std::list<SvdInterrupt *> &interrupts);
  json AddressBlocksToJson(SvdPeripheral *const peri);
  json InterruptsToJson(const std::list<SvdInterrupt *> &interrupts);
  json RegistersContainerToJson(SvdPeripheral *const peri);
  json RegistersClustersToJson(const std::__cxx11::list<SvdItem *> &registersClusters);
  json RegisterToJson(SvdRegister *reg);
  json ClusterToJson(SvdCluster *cluster);
  json FieldToJson(SvdField *field);
  json EnumContainerToJson(SvdEnumContainer *const enumContainer);
  json EnumToJson(SvdEnum *const &enum_);
  void PrintDeviceJson(SvdDevice *device);

protected:
  bool InitMessageTable();

private:
  SvdOptions m_svdOptions;

  static const MsgTable msgTable;
  static const MsgTableStrict msgStrictTable;

};

#endif // SVDConv_H
