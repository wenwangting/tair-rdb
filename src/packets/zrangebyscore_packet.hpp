/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * zrangebyscore packet
 *
 * Version: $Id: zrangebyscore_packet.hpp 28 2010-09-19 05:18:09Z ruohai@taobao.com $
 *
 * Authors:
 *   ruohai <ruohai@taobao.com>
 *     - initial release
 *
 */
#ifndef TAIR_PACKET_ZRANGEBYSCORE_PACKET_H
#define TAIR_PACKET_ZRANGEBYSCORE_PACKET_H

#include <stdint.h>

#include "base_packet.hpp"
#include "storage_manager.hpp"

#include <tbsys.h>
#include <sstream>
#include <iomanip>

namespace tair
{
    inline std::string hexStr(char *data, int len)
    {
        std::stringstream ss;
        ss << std::hex;
        ss << std::setfill('0') << std::setw(2);
        for(int i(0); i < len; ++i)
            ss << "\\x" << (unsigned int)data[i];
        return ss.str();
    }
    
    class request_zrangebyscore: public base_packet
    {
    public:
        request_zrangebyscore ()
        {
            setPCode (TAIR_REQ_ZRANGEBYSCORE_PACKET);

            server_flag = 0;
            area = 0;
            start = 0.0;
            end = 0.0;
            withscore = 0;
        }

        request_zrangebyscore (request_zrangebyscore &packet)
        {
            setPCode (packet.getPCode ());
            server_flag = packet.server_flag;
            area = packet.area;
            start = packet.start;
            end = packet.end;
            withscore = packet.withscore;
            key.clone (packet.key);
        }

        bool encode (tbnet::DataBuffer *output)
        {
            CREATE_HEADER;

            PUT_DOUBLE_TO_BUFFER (output, start);
            PUT_DOUBLE_TO_BUFFER (output, end);
            PUT_DATAENTRY_TO_BUFFER (output, key);
            PUT_INT32_TO_BUFFER(output, withscore);

            return true;
        }

        bool decode (tbnet::DataBuffer *input, tbnet::PacketHeader *header)
        {
            HEADER_VERIFY;

            GETKEY_FROM_DOUBLE (input, start);
            GETKEY_FROM_DOUBLE (input, end);
            GETKEY_FROM_DATAENTRY (input, key);

            // js发送zrangebyscore请求时如果withscore=0则请求包中是没有withscore字段的
            // 如果withscore=1才在请求包中追加withscore，所以为了兼容js需要做出判断

            if(input->getDataLen() == 4)
            {
                withscore = input->readInt32();
            }

            return true;
        }

    public:
        uint16_t area;
        double start;
        double end;
        int withscore;
        data_entry key;
    };

    #define RESPONSE_VALUES_MAXSIZE 32767

    class response_zrangebyscore: public base_packet
    {
    public:
        response_zrangebyscore ()
        {
            setPCode (TAIR_RESP_ZRANGEBYSCORE_PACKET);
            config_version = 0;
            sfree = 1;
        }

        ~response_zrangebyscore ()
        {
            CLEAR_DATA_VECTOR(values, sfree);
        }

        bool encode (tbnet::DataBuffer *output)
        {
            if (values.size () > RESPONSE_VALUES_MAXSIZE) return false;

            PUT_INT32_TO_BUFFER(output, config_version);
            PUT_INT16_TO_BUFFER(output, version);
            PUT_INT32_TO_BUFFER(output, code);
            PUT_DATAVECTOR_TO_BUFFER(output, values);

            // if scores is not empty
            // then his size must be equal to values's size

            int count = (int)scores.size();
            PUT_INT32_TO_BUFFER(output, count);

            if(count)
            {
                for(size_t i = 0; i < count; ++i)
                {
                    PUT_DOUBLE_TO_BUFFER(output, scores[i]);
                }
            }

            return true;
        }

        bool decode (tbnet::DataBuffer *input, tbnet::PacketHeader *header)
        {
            GETKEY_FROM_INT32(input, config_version);
            GETKEY_FROM_INT16(input, version);
            GETKEY_FROM_INT32(input, code);
            GETKEY_FROM_DATAVECTOR(input, values);

            int count;
            GETKEY_FROM_INT32(input, count);

            if(count)
            {
                for(int i = 0; i < count; ++i)
                {
                    double score;
                    GETKEY_FROM_DOUBLE(input, score);
                    scores.push_back(score);
                }
            }

            for(size_t i = 0; i < values.size(); i++)
            {
                values[i]->set_version(version);
            }

            return true;
        }

        void set_meta (uint32_t config_version, uint32_t code)
        {
            this->config_version = config_version;
            //this->version           = version;
            this->code = code;
        }

        void set_version(uint16_t version)
        {
            this->version = version;
        }

        int get_code ()
        {
            return code;
        }

        void set_code (int cde)
        {
            code = cde;
        }


        void add_data (data_entry *data)
        {
            if (data == NULL)
                return;
            values.push_back (data);
        }

        void alloc_free(int ifree)
        {
            sfree = ifree;
        }

    public:
        uint16_t version;
        uint32_t config_version;
        int32_t code;
        int sfree;
        vector<data_entry *> values;
        vector<double> scores;
    };
}       // end namespace
#endif
