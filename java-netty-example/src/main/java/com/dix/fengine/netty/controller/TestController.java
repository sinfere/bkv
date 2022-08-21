package com.dix.fengine.netty.controller;

import com.dix.codec.bkv.BKV;
import com.dix.codec.bkv.CodecUtil;
import com.dix.fengine.netty.bean.DataResponse;
import com.dix.fengine.netty.client.ClientRegistry;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.io.IOException;


@RestController
@RequestMapping("/test")
public class TestController  {
    private static Logger logger = LoggerFactory.getLogger(TestController.class);

    @RequestMapping("/empty")
    public DataResponse test() {
        return DataResponse.create();
    }

    @RequestMapping("/write")
    public DataResponse write(
            @RequestParam("client_id") String clientId,
            @RequestParam("data") String data
    ) {
        byte[] buf = CodecUtil.hexToBytes(data);
        ClientRegistry.getInstance().write(clientId, buf);
        return DataResponse.create();
    }

    @RequestMapping("/write/bkv")
    public DataResponse write(
            @RequestParam("client_id") String clientId
    ) throws IOException {
        BKV bkv = new BKV();
        bkv.add(1, 1);
        bkv.add(2, "demo");
        bkv.add(3, "hello, world");
        ClientRegistry.getInstance().write(clientId, bkv);
        return DataResponse.create();
    }



}
