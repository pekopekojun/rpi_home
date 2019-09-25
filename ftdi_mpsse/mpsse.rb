# coding: UTF-8

require 'ffi'

module Ftd2xx
  FT_OK = 0
  FT_INVALID_HANDLE = 1
  FT_DEVICE_NOT_FOUND = 2
  FT_DEVICE_NOT_OPENED = 3
  FT_IO_ERROR = 4
  FT_INSUFFICIENT_RESOURCES = 5
  FT_INVALID_PARAMETER = 6
  FT_INVALID_BAUD_RATE = 7

  FT_DEVICE_NOT_OPENED_FOR_ERASE = 8
  FT_DEVICE_NOT_OPENED_FOR_WRITE = 9
  FT_FAILED_TO_WRITE_DEVICE = 10
  FT_EEPROM_READ_FAILED = 11
  FT_EEPROM_WRITE_FAILED = 12
  FT_EEPROM_ERASE_FAILED = 13
  FT_EEPROM_NOT_PRESENT = 14
  FT_EEPROM_NOT_PROGRAMMED = 15
  FT_INVALID_ARGS = 16
  FT_NOT_SUPPORTED = 17
  FT_OTHER_ERROR = 18
  FT_DEVICE_LIST_NOT_READY = 19

  extend FFI::Library

  class FT_DEVICE_LIST_INFO_NODE < FFI::Struct
    layout(
      :Flags, :ulong,
      :Type, :ulong,
      :ID, :ulong,
      :LocId, :ulong,
      :SerialNumber, [:char, 16],
      :Description, [:char, 64],
      :ftHandle, :pointer,
    )
  end

  ffi_lib "ftd2xx.dll"
  attach_function :FT_Open, [:int, :pointer], :ulong
  attach_function :FT_Close, [:pointer], :ulong
  attach_function :FT_ResetDevice, [:pointer], :ulong
  attach_function :FT_GetQueueStatus, [:pointer, :pointer], :ulong
  attach_function :FT_SetUSBParameters, [:pointer, :int, :int], :ulong
  attach_function :FT_SetChars, [:pointer, :int, :int, :int, :int], :ulong
  attach_function :FT_SetTimeouts, [:pointer, :int, :int], :ulong
  attach_function :FT_SetLatencyTimer, [:pointer, :int], :ulong
  attach_function :FT_SetBitMode, [:pointer, :int, :int], :ulong
  attach_function :FT_GetDeviceInfo, [:pointer, :pointer, :pointer, :pointer, :pointer, :pointer], :ulong
  attach_function :FT_CreateDeviceInfoList, [:pointer], :ulong
  attach_function :FT_GetDeviceInfoList, [:pointer, :pointer], :ulong
  attach_function :FT_Read, [:pointer, :pointer, :int, :pointer], :ulong
  attach_function :FT_Write, [:pointer, :pointer, :ulong, :pointer], :ulong
end

class Mpsse
  MSB_FALLING_EDGE_CLOCK_BYTE_OUT = 0x11
  MSB_RISING_EDGE_CLOCK_BIT_IN = 0x22
  MSB_RISING_EDGE_CLOCK_BYTE_IN = 0x20
  MSB_FALLING_EDGE_CLOCK_BYTE_IN = 0x24
  SET_DATA_BITS_LOW_BYTE = 0x80   # ADBUS
  READ_DATA_BITS_LOW_BYTE = 0x81  # ADBUS
  SET_DATA_BITS_HIGH_BYTE = 0x82  # ACBUS
  READ_DATA_BITS_HIGH_BYTE = 0x83 # ACBUS
  PORT_SPI_SCLK = 0x01
  PORT_SPI_MISO = 0x02
  PORT_SPI_MOSI = 0x04
  PORT_SPI_CS_HIGH = 0x08
  PORT_SPI_CS_LOW = 0x00

  PORT_SCLK = 0x01
  PORT_DO = 0x02

  SPI_CS_LOW = 1
  SPI_CS_HIGH = 0

  SPI_1200KHZ = 4
  SPI_1500KHZ = 3
  SPI_2000KHZ = 2
  SPI_3000KHZ = 1
  SPI_6000KHZ = 0

  I2C_400KHZ = 14
  I2C_375KHZ = 15
  I2C_300KHZ = 19
  I2C_200KHZ = 29
  I2C_100KHZ = 59

  def initialize
    @adbus_dir = 0
    @acbus_dir = 0
    @ftHandle = nil
  end

  def open(dev_num = 0)
    @ftHandle = FFI::MemoryPointer.new :pointer
    inputSize = FFI::MemoryPointer.new :pointer
    inputRead = FFI::MemoryPointer.new :pointer
    inputBuffer = FFI::MemoryPointer.new :char, 1024

    ft_status = Ftd2xx::FT_OK

    ft_status = Ftd2xx.FT_Open(dev_num, @ftHandle)
    return ft_status if ft_status != Ftd2xx::FT_OK

    ft_status = Ftd2xx.FT_ResetDevice(@ftHandle.read_pointer)
    return ft_status if ft_status != Ftd2xx::FT_OK

    ft_status = Ftd2xx.FT_GetQueueStatus(@ftHandle.read_pointer, inputSize)
    return ft_status if ft_status != Ftd2xx::FT_OK

    if inputSize.read_int > 0
      ft_status = Ftd2xx.FT_Read(@ftHandle.read_pointer, inputBuffer, inputSize.read_int, inputRead)
    end
    return ft_status if ft_status != Ftd2xx::FT_OK

    ft_status = Ftd2xx.FT_SetUSBParameters(@ftHandle.read_pointer, 65536, 65535)
    return ft_status if ft_status != Ftd2xx::FT_OK

    ft_status = Ftd2xx.FT_SetChars(@ftHandle.read_pointer, 0, 0, 0, 0)
    return ft_status if ft_status != Ftd2xx::FT_OK

    ft_status = Ftd2xx.FT_SetTimeouts(@ftHandle.read_pointer, 1000, 1000)
    return ft_status if ft_status != Ftd2xx::FT_OK

    ft_status = Ftd2xx.FT_SetLatencyTimer(@ftHandle.read_pointer, 1)
    return ft_status if ft_status != Ftd2xx::FT_OK

    ft_status = Ftd2xx.FT_SetBitMode(@ftHandle.read_pointer, 0, 0)
    return ft_status if ft_status != Ftd2xx::FT_OK

    ft_status = Ftd2xx.FT_SetBitMode(@ftHandle.read_pointer, 0, 2)
    return ft_status if ft_status != Ftd2xx::FT_OK

    ft_status
  end

  def close
    Ftd2xx.FT_Close(@ftHandle)
  end

  def device_info
    ft_dev = FFI::MemoryPointer.new :ulong
    device_id = FFI::MemoryPointer.new :ulong
    serial_number = FFI::MemoryPointer.new :char, 16
    desc = FFI::MemoryPointer.new :char, 1024

    ft_status = Ftd2xx.FT_GetDeviceInfo(@ftHandle.read_pointer, ft_dev, device_id, serial_number, desc, nil)
    raise "Error #{ft_status}" if ft_status != Ftd2xx::FT_OK

    return {"serial_number" => serial_number.read_string,
            "description" => desc.read_string,
            "device_id" => device_id.read_int,
            "device" => ft_dev.read_int}
  end

  def device_list
    num_devs = FFI::MemoryPointer.new :ulong
    ft_status = Ftd2xx.FT_CreateDeviceInfoList(num_devs)
    devInfo = FFI::MemoryPointer.new Ftd2xx::FT_DEVICE_LIST_INFO_NODE, num_devs.read_ulong

    ft_status = Ftd2xx.FT_GetDeviceInfoList(devInfo, num_devs)
    ary = []
    num_devs.read_ulong.times do |idx|
      dev = Ftd2xx::FT_DEVICE_LIST_INFO_NODE.new(devInfo[idx])
      hash = Hash.new
      hash["dev_num"] = idx
      hash["serial_number"] = dev[:SerialNumber].to_s
      hash["description"] = dev[:Description].to_s
      hash["flags"] = dev[:Flags].to_i

      ary.push(hash)
    end
    ary
  end

  def adbus_set_dir(dir, val)
    @adbus_dir = dir
    adbus_write(val)
  end

  def adbus_read
    dummy = FFI::MemoryPointer.new :ulong

    dat = FFI::MemoryPointer.new :uchar, 1
    dat.put_array_of_uchar(0, [0x81])
    ft_status = Ftd2xx.FT_Write(@ftHandle.read_pointer, dat, 1, dummy)
    raise ft_status if ft_status != Ftd2xx::FT_OK

    ret = FFI::MemoryPointer.new :uchar, 1
    ft_status = Ftd2xx.FT_Read(@ftHandle.read_pointer, ret, 1, dummy)
    raise ft_status if ft_status != Ftd2xx::FT_OK

    ret.get_array_of_uchar(0, 1)
  end

  def adbus_write(val)
    dummy = FFI::MemoryPointer.new :ulong

    dat = FFI::MemoryPointer.new :uchar, 3
    dat.put_array_of_uchar(0, [0x80, val, @adbus_dir])
    ft_status = Ftd2xx.FT_Write(@ftHandle.read_pointer, dat, 3, dummy)
    raise ft_status if ft_status != Ftd2xx::FT_OK
    ft_status
  end

  def acbus_set_dir(dir, val)
    @acbus_dir = dir
    acbus_write(val)
  end

  def acbus_read
    dummy = FFI::MemoryPointer.new :ulong

    dat = FFI::MemoryPointer.new :uchar, 1
    dat.put_array_of_uchar(0, [0x83])

    ft_status = Ftd2xx.FT_Write(@ftHandle.read_pointer, dat, 1, dummy)
    raise ft_status if ft_status != Ftd2xx::FT_OK

    ret = FFI::MemoryPointer.new :uchar, 1
    ft_status = Ftd2xx.FT_Read(@ftHandle.read_pointer, ret, 1, dummy)
    raise ft_status if ft_status != Ftd2xx::FT_OK

    ret.get_array_of_uchar(0, 1)[0]
  end

  def acbus_write(val)
    dummy = FFI::MemoryPointer.new :ulong

    dat = FFI::MemoryPointer.new :uchar, 3
    dat.put_array_of_uchar(0, [0x82])
    dat.put_array_of_uchar(1, [val])
    dat.put_array_of_uchar(2, [@acbus_dir])
    ft_status = Ftd2xx.FT_Write(@ftHandle.read_pointer, dat, 3, dummy)
    raise ft_status if ft_status != Ftd2xx::FT_OK
    ft_status
  end
end
