package com.dix.fengine.netty.controller;

import com.dix.fengine.netty.bean.ErrorResponse;
import com.dix.fengine.netty.exception.BaseException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.core.MethodParameter;
import org.springframework.http.HttpStatus;
import org.springframework.web.bind.MissingServletRequestParameterException;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.method.annotation.MethodArgumentTypeMismatchException;

import javax.servlet.http.HttpServletRequest;

@ControllerAdvice
public class ExceptionHandlerController {

    private static Logger logger = LoggerFactory.getLogger(ExceptionHandlerController.class);

    @ExceptionHandler(value = {Exception.class, RuntimeException.class, Throwable.class})
    @ResponseBody
    @ResponseStatus(HttpStatus.OK)
    public ErrorResponse defaultErrorHandler(HttpServletRequest request, Exception e, Throwable ex) {
        ErrorResponse errorResponse = new ErrorResponse(BaseException.ERROR_IN_INTERCEPTOR, "system error");

        if (e != null) {
            errorResponse.setMessage(e.getMessage());
            e.printStackTrace();

            if (e instanceof MissingServletRequestParameterException) {
                MissingServletRequestParameterException missingServletRequestParameterException = (MissingServletRequestParameterException) e;
                errorResponse.setCode(BaseException.ERROR_PARAM_NOT_SET);
                errorResponse.setMessage(String.format("param [%s] required", missingServletRequestParameterException.getParameterName()));
                return errorResponse;
            }

            if (e instanceof MethodArgumentTypeMismatchException) {
                MethodArgumentTypeMismatchException methodArgumentTypeMismatchException = (MethodArgumentTypeMismatchException) e;
                errorResponse.setCode(BaseException.ERROR_WRONG_PARAM);

                MethodParameter methodParameter = methodArgumentTypeMismatchException.getParameter();
                RequestParam paramAnnotation = methodParameter.getParameterAnnotation(RequestParam.class);
                errorResponse.setMessage(String.format("param [%s] type mismatch", paramAnnotation == null ? methodParameter.getParameterName() : paramAnnotation.value()));
                return errorResponse;
            }

            if (e instanceof BaseException) {
                BaseException baseException = (BaseException) e;
                errorResponse.setCode(baseException.getCode());
                return errorResponse;
            }

        }

        return errorResponse;
    }
}