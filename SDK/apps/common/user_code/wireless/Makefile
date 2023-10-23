# ifdef CONFIG_CPU_BR29
ifeq ($(findstring CONFIG_CPU_BR29, $(DEFINES)),)

else

INCLUDES += \
	-I../../sdk/application/wireless

c_SRC_FILES += \
	../../sdk/application/wireless/u_wireless.c\
	../../sdk/application/wireless/u_wireless_list.c

endif

# ifdef CONFIG_CPU_BR30
ifeq ($(findstring CONFIG_CPU_BR30, $(DEFINES)),)

else
INCLUDES += \
	-I../../../../../../sdk/application/wireless

c_SRC_FILES += \
	../../../../../../sdk/application/wireless/u_wireless.c\
	../../../../../../sdk/application/wireless/u_wireless_list.c

endif