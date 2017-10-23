#include <app_error.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>


void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info_)
{
    auto& info = *reinterpret_cast<error_info_t*>(info_);
    auto errStr = nrf_strerror_get(info.err_code);

    NRF_LOG_ERROR("Fatal error in %s line %d with code %d (%s)",
        (const char*)info.p_file_name,
        (int)info.line_num,
        (int)info.err_code,
        errStr
    );
    NRF_LOG_FINAL_FLUSH();
    NRF_BREAKPOINT_COND;
    // On assert, the system can only recover with a reset.
#ifndef DEBUG
    NRF_LOG_INFO("Hit weak handler");
    NVIC_SystemReset();
#else
    app_error_save_and_stop(id, pc, info_);
#endif // DEBUG
}
