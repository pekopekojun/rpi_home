require_relative 'mpsse.rb'

class FT232H_I2C < Mpsse
  @slaveAddress = 0

  def initialize; end

  def cmd_start_condition
    [
      # Keep CLK=H, DO=H
      SET_DATA_BITS_LOW_BYTE, PORT_DO | PORT_SCLK, 0x03,
      # Make DO down edge while CLK=H
      SET_DATA_BITS_LOW_BYTE, PORT_SCLK, 0x03,
      # Set to Initial condition
      SET_DATA_BITS_LOW_BYTE, 0x00, 0x03
    ]
  end

  def cmd_stop_condition
    [
      # Make DO down edge while CLK=H
      SET_DATA_BITS_LOW_BYTE, PORT_SCLK, 0x03, SET_DATA_BITS_LOW_BYTE, PORT_SCLK | PORT_DO, 0x03
    ]
  end

  def cmd_ack
    [
      # InDirection
      SET_DATA_BITS_LOW_BYTE, 0x00, PORT_SCLK, MSB_RISING_EDGE_CLOCK_BYTE_IN, 0x00, 0x00,
      # Send xAck
      SET_DATA_BITS_LOW_BYTE, 0x00, PORT_SCLK | PORT_DO, SET_DATA_BITS_LOW_BYTE, PORT_SCLK,
      PORT_SCLK | PORT_DO, SET_DATA_BITS_LOW_BYTE, 0x00, PORT_SCLK | PORT_DO,
      # Out Direction
      SET_DATA_BITS_LOW_BYTE, 0x00, PORT_SCLK | PORT_DO
    ]
  end

  def cmd_nack
    [
      # InDirection
      SET_DATA_BITS_LOW_BYTE, 0x00, PORT_SCLK, MSB_RISING_EDGE_CLOCK_BYTE_IN, 0x00, 0x00,
      # Send xAck
      SET_DATA_BITS_LOW_BYTE, PORT_SCLK | PORT_DO, PORT_SCLK | PORT_DO, SET_DATA_BITS_LOW_BYTE,
      PORT_SCLK | PORT_DO, PORT_SCLK | PORT_DO, SET_DATA_BITS_LOW_BYTE, PORT_DO,
      PORT_SCLK | PORT_DO,
      # Out Direction
      SET_DATA_BITS_LOW_BYTE, 0x00, PORT_SCLK | PORT_DO
    ]
  end

  def cmd_send(saddr)
    [
      # Data
      MSB_FALLING_EDGE_CLOCK_BYTE_OUT, 0x00, 0x00, saddr,
      # Get Ack
      SET_DATA_BITS_LOW_BYTE, 0x00, 0x01,     # SCLK:out, DO:in, DI:in
      MSB_RISING_EDGE_CLOCK_BIT_IN, 0x00,
      0x87,                                   # Send Immediate
      SET_DATA_BITS_LOW_BYTE, PORT_DO, 0x03,  # SCLK:out, DO:out, DI:in
    ]
  end

  def cmd_send_read(saddr)
    [
      # Data
      MSB_FALLING_EDGE_CLOCK_BYTE_OUT, 0x00, 0x00, saddr,
      # Get Ack
      SET_DATA_BITS_LOW_BYTE, 0x00, 0x01,  # SCLK:out, DO:in, DI:in
      MSB_RISING_EDGE_CLOCK_BIT_IN, 0x00,
      0x87,                                # / Send Immediate
      SET_DATA_BITS_LOW_BYTE, 0x00, 0x03,  # SCLK:out, DO:out, DI:in
    ]
  end

  def i2c_send_cmd(cmd)
    dummy = FFI::MemoryPointer.new :ulong
    dat = FFI::MemoryPointer.new :uchar, cmd.size
    # cmd.size.times do |idx|
    #   dat.put_array_of_uchar(idx, [cmd[idx]])
    # end
    dat.put_array_of_uchar(0, cmd)
    ret = Ftd2xx.FT_Write(@ftHandle.read_pointer, dat, cmd.size, dummy)

    ret
  end

  def check_ack
    dummy = FFI::MemoryPointer.new :ulong
    dummy2 = FFI::MemoryPointer.new :uchar, 1
    ft_status = Ftd2xx.FT_Read(@ftHandle.read_pointer, dummy2, 1, dummy)
    raise "Error !! Check ack #{ft_status}" if ft_status != Ftd2xx::FT_OK
    raise "Error !! No ack #{ft_status}" if dummy2.read_uchar & 0x01 == 1
  end

  def init(saddr, speed)
    cmd = FFI::MemoryPointer.new :uchar, 16
    dummy = FFI::MemoryPointer.new :ulong

    @slaveAddress = saddr

    cmd.put_array_of_uchar(0, [0x85]) # Disconnect TDI to TDO for Loopback
    cmd.put_array_of_uchar(1, [0x97]) # Turn Off Adaptive clocking
    cmd.put_array_of_uchar(2, [0x8c]) # Enable 3 Phase Data Clocking

    ## Set clk divisor (FT232H/FT2232H/FT4232H)
    cmd.put_array_of_uchar(3, [0x86]) #  Set clk divisor (FT232H/FT2232H/FT4232H)

    # TCK/SK period = 12MHz / (( 1 +((ValueH * 256) | ValueL) ) * 2)
    cmd.put_array_of_uchar(4, [speed]) # ValueL
    cmd.put_array_of_uchar(5, [0])     # ValueH

    cmd.put_array_of_uchar(6, [SET_DATA_BITS_LOW_BYTE])
    cmd.put_array_of_uchar(7, [0x03])
    cmd.put_array_of_uchar(8, [0x03])

    ft_status = Ftd2xx.FT_Write(@ftHandle.read_pointer, cmd, 9, dummy)

    ft_status
  end

  def read(sadr = @slaveAddress, adr, size)
    # send slave adress
    cmd = []
    cmd.concat(cmd_start_condition)
    cmd.concat(cmd_send(sadr))
    ft_status = i2c_send_cmd(cmd)
    check_ack
    raise "Error !! Send slave address #{ft_status}" if ft_status != Ftd2xx::FT_OK

    # send reg adress
    cmd = []
    cmd.concat(cmd_send(adr))
    ft_status = i2c_send_cmd(cmd)
    check_ack
    raise "Error !! Send address #{ft_status}" if ft_status != Ftd2xx::FT_OK

    # send read request
    cmd = []
    cmd.concat(cmd_start_condition)
    cmd.concat(cmd_send_read(sadr | 0x01))
    ft_status = i2c_send_cmd(cmd)
    raise "Error !! Send read req #{ft_status}" if ft_status != Ftd2xx::FT_OK
    check_ack

    # send ack/nack/stop
    cmd = []
    (size - 1).times do
      cmd.concat(cmd_ack)
    end
    cmd.concat(cmd_nack)
    cmd.concat(cmd_stop_condition)
    ft_status = i2c_send_cmd(cmd)
    raise "Error !! xack/stop condition #{ft_status}" if ft_status != Ftd2xx::FT_OK

    # read data
    ret = FFI::MemoryPointer.new :uchar, size
    dummy = FFI::MemoryPointer.new :ulong
    ft_status = Ftd2xx.FT_Read(@ftHandle.read_pointer, ret, size, dummy)
    raise "Error !! read data #{ft_status}" if ft_status != Ftd2xx::FT_OK

    ret.get_array_of_uchar(0, size)
  end

  def write(sadr = @slaveAddress, adr, dat)
    # send slave adress
    cmd = []
    cmd.concat(cmd_start_condition)
    cmd.concat(cmd_send(sadr))
    ft_status = i2c_send_cmd(cmd)
    raise "Error !! Send slave address #{ft_status}" if ft_status != Ftd2xx::FT_OK
    check_ack

    writedata = [adr]
    writedata.concat(dat)

    # write
    writedata.size.times do |idx|
      cmd = []
      cmd.concat(cmd_send(writedata[idx]))
      ft_status = i2c_send_cmd(cmd)
      raise "Error !! Send write data #{ft_status}" if ft_status != Ftd2xx::FT_OK
      check_ack
    end

    # stop
    cmd = []
    cmd.concat(cmd_stop_condition)
    ft_status = i2c_send_cmd(cmd)
    raise "Error !! stop condition #{ft_status}" if ft_status != Ftd2xx::FT_OK

    ft_status
  end
end
